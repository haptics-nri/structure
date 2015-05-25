/*****************************************************************************
*                                                                            *
*  OpenNI 2.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
#include <OpenNI.h>
#include <string>
#include <fstream>
#include <png++/png.hpp>

class pixel_generator
    : public png::generator< png::gray_pixel_1, pixel_generator >
{
public:
    pixel_generator(openni::VideoFrameRef& frame)
        : png::generator< png::gray_pixel_1, pixel_generator >(frame.getWidth(), frame.getHeight()),
	  m_frame(frame),
	  m_buf((openni::DepthPixel*)frame.getData())
    {
    	printf("VideoFrameRef w=%d h=%d s=%d ds=%d\n", frame.getWidth(), frame.getHeight(), frame.getStrideInBytes(), frame.getDataSize());
    }

    png::byte* get_next_row(size_t pos)
    {
	return reinterpret_cast<png::byte*>(&m_buf[pos*m_frame.getWidth()]);
    }

private:
	openni::VideoFrameRef& m_frame;
	openni::DepthPixel *m_buf;
};

int main(int argc, char** argv)
{
	openni::Status rc = openni::STATUS_OK;

	openni::Device device;
	openni::VideoStream depth;
	const char* deviceURI = openni::ANY_DEVICE;
	if (argc > 1)
	{
		deviceURI = argv[1];
	}

	rc = openni::OpenNI::initialize();
	if (rc != openni::STATUS_OK)
	{
		fprintf(stderr, "logger: After initialization:\n%s\n", openni::OpenNI::getExtendedError());
		return 1;
	}

	rc = device.open(deviceURI);
	if (rc != openni::STATUS_OK)
	{
		fprintf(stderr, "logger: Device open failed:\n%s\n", openni::OpenNI::getExtendedError());
		openni::OpenNI::shutdown();
		return 3;
	}

	rc = depth.create(device, openni::SENSOR_DEPTH);
	if (rc == openni::STATUS_OK)
	{
		rc = depth.start();
		if (rc != openni::STATUS_OK)
		{
			fprintf(stderr, "logger: Couldn't start depth stream:\n%s\n", openni::OpenNI::getExtendedError());
			depth.destroy();
		}
	}
	else
	{
		fprintf(stderr, "logger: Couldn't find depth stream:\n%s\n", openni::OpenNI::getExtendedError());
	}

	if (!depth.isValid())
	{
		fprintf(stderr, "logger: No valid streams. Exiting\n");
		openni::OpenNI::shutdown();
		return 4;
	}

	int idx;
	openni::VideoFrameRef frame;
	openni::DepthPixel* buf;
	std::ofstream file;
	png::image<png::gray_pixel> image;
	for (int i = 0; i < 10; ++i)
	{
		fprintf(stderr, "logger: Waiting for frame %d\n", i);
		
		depth.readFrame(&frame);
		fprintf(stderr, "logger: Got frame\n");
		buf = (openni::DepthPixel*)frame.getData();
		fprintf(stderr, "logger: Opening file\n");
		file.open((std::string("log") + (char)(i+'0') + ".png").c_str(), std::ios::out | std::ios::binary);
		if (!file.good())
		{
			fprintf(stderr, "logger: failed to open PNG file for writing. Exiting\n");
			return 5;
		}
		fprintf(stderr, "logger: Writing image\n");
		pixel_generator(frame).write(file);
		file.close();

		printf("logger: Received frame %d (%d x %d px)\n", i, frame.getHeight(), frame.getWidth());
	}

	depth.stop();
	device.close();
	openni::OpenNI::shutdown();
	return 0;
}
