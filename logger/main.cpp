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
#include <csignal>

bool g_running = true;

void signal_handler(int signal)
{
	fprintf(stderr, "Signal received! Cleaning up.\n");
	g_running = false;
}

int main(int argc, char** argv)
{
	signal(SIGABRT, signal_handler);
	signal(SIGINT,  signal_handler);
	signal(SIGTERM, signal_handler);

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
	char fname[50];
	timespec now;
	for (unsigned long i = 0; g_running; ++i)
	{
		fprintf(stderr, "logger: Waiting for frame %ld\n", i);
		
		depth.readFrame(&frame);
		fprintf(stderr, "logger: Got frame\n");
		buf = (openni::DepthPixel*)frame.getData();
		fprintf(stderr, "logger: Opening file\n");
		clock_gettime(CLOCK_BOOTTIME, &now);
		if (snprintf(fname, sizeof fname, "structure-%ld.%09ld.csv", now.tv_sec, now.tv_nsec) >= sizeof fname)
		{
			fprintf(stderr, "logger: generated filename would be too long. Exiting\n");
			return 5;
		}
		file.open(fname, std::ios::out);
		if (!file.good())
		{
			fprintf(stderr, "logger: failed to open PNG file for writing. Exiting\n");
			return 6;
		}
		fprintf(stderr, "logger: Writing image\n");
		for (int y = 0; y < frame.getHeight(); ++y)
		{
			for (int x = 0; x < frame.getWidth(); ++x)
			{
				file << ((openni::DepthPixel*)frame.getData())[y*frame.getWidth() + x];
				if (x < frame.getWidth()-1) file << ",";
			}
			file << "\n";
		}
		file.close();

		fprintf(stderr, "logger: Received frame %ld (%d x %d px)\n", i, frame.getHeight(), frame.getWidth());
	}

	fprintf(stderr, "Bye!\n");
	depth.stop();
	device.close();
	openni::OpenNI::shutdown();
	return 0;
}
