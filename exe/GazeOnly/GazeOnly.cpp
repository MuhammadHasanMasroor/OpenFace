///////////////////////////////////////////////////////////////////////////////
// Minimal gaze-only demo using OpenFace LandmarkDetector + GazeAnalyser.
///////////////////////////////////////////////////////////////////////////////

#include "LandmarkCoreIncludes.h"
#include "GazeEstimation.h"

#include <SequenceCapture.h>
#include <Visualizer.h>
#include <VisualizationUtils.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static std::vector<std::string> get_arguments(int argc, char** argv)
{
	std::vector<std::string> arguments;
	for (int i = 0; i < argc; ++i)
	{
		arguments.emplace_back(argv[i]);
	}
	return arguments;
}

struct AppOptions
{
	bool show = false;
	int max_frames = -1; // <=0 means no limit
};

static AppOptions consume_app_args(std::vector<std::string>& arguments)
{
	AppOptions opts;

	std::vector<bool> keep(arguments.size(), true);
	for (size_t i = 0; i < arguments.size(); ++i)
	{
		if (arguments[i] == "-show")
		{
			opts.show = true;
			keep[i] = false;
		}
		else if (arguments[i] == "-max_frames" && i + 1 < arguments.size())
		{
			std::stringstream ss(arguments[i + 1]);
			ss >> opts.max_frames;
			keep[i] = false;
			keep[i + 1] = false;
			++i;
		}
	}

	for (int i = (int)arguments.size() - 1; i >= 0; --i)
	{
		if (!keep[(size_t)i])
		{
			arguments.erase(arguments.begin() + i);
		}
	}

	return opts;
}

static void print_usage(const char* exe)
{
	std::cout
		<< "Usage:\n"
		<< "  " << exe << " [OpenFace input args] -mloc model/main_clnf_general.txt [-show] [-max_frames N]\n\n"
		<< "Input args (same as OpenFace executables):\n"
		<< "  -device 0                Webcam index\n"
		<< "  -f <video_path>          Video file\n"
		<< "  -fdir <image_dir>        Image sequence directory\n"
		<< "  -fx/-fy/-cx/-cy <float>  Camera intrinsics (optional)\n\n"
		<< "Required for gaze:\n"
		<< "  -mloc model/main_clnf_general.txt  (CLNF model with eye parts)\n\n"
		<< "App-only args:\n"
		<< "  -show              Show a preview window (press 'q' to quit)\n"
		<< "  -max_frames N      Stop after N frames (useful for webcam)\n";
}

int main(int argc, char** argv)
{
	std::vector<std::string> arguments = get_arguments(argc, argv);
	if (arguments.size() == 1)
	{
		print_usage(argv[0]);
		return 0;
	}

	AppOptions opts = consume_app_args(arguments);

	LandmarkDetector::FaceModelParameters det_parameters(arguments);
	LandmarkDetector::CLNF face_model(det_parameters.model_location);
	if (!face_model.loaded_successfully)
	{
		std::cerr << "ERROR: Could not load the landmark detector model.\n";
		std::cerr << "Tip: for gaze use `-mloc model/main_clnf_general.txt`.\n";
		return 1;
	}

	if (!face_model.eye_model)
	{
		std::cerr << "WARNING: loaded model has no eye model; gaze will stay zero.\n";
		std::cerr << "Tip: use `-mloc model/main_clnf_general.txt`.\n";
	}

	Utilities::SequenceCapture sequence_reader;
	Utilities::Visualizer visualizer(true, false, false, false);
	Utilities::FpsTracker fps_tracker;
	fps_tracker.AddFrame();

	std::cout << "frame,timestamp,gaze_angle_x,gaze_angle_y,"
		<< "gaze_0_x,gaze_0_y,gaze_0_z,"
		<< "gaze_1_x,gaze_1_y,gaze_1_z\n";

	int total_frame_counter = 0;

	while (true)
	{
		if (!sequence_reader.Open(arguments))
			break;

		cv::Mat rgb_image = sequence_reader.GetNextFrame();
		while (!rgb_image.empty())
		{
			cv::Mat_<uchar> grayscale_image = sequence_reader.GetGrayFrame();
			bool detection_success = LandmarkDetector::DetectLandmarksInVideo(rgb_image, face_model, det_parameters, grayscale_image);

			cv::Point3f gaze0(0, 0, 0);
			cv::Point3f gaze1(0, 0, 0);
			cv::Vec2f gaze_angle(0, 0);

			if (detection_success && face_model.eye_model)
			{
				GazeAnalysis::EstimateGaze(face_model, gaze0, sequence_reader.fx, sequence_reader.fy, sequence_reader.cx, sequence_reader.cy, true);
				GazeAnalysis::EstimateGaze(face_model, gaze1, sequence_reader.fx, sequence_reader.fy, sequence_reader.cx, sequence_reader.cy, false);
				gaze_angle = GazeAnalysis::GetGazeAngle(gaze0, gaze1);
			}

			std::cout
				<< sequence_reader.GetFrameNumber() << ","
				<< sequence_reader.time_stamp << ","
				<< gaze_angle[0] << "," << gaze_angle[1] << ","
				<< gaze0.x << "," << gaze0.y << "," << gaze0.z << ","
				<< gaze1.x << "," << gaze1.y << "," << gaze1.z
				<< "\n";

			if (opts.show)
			{
				fps_tracker.AddFrame();

				visualizer.SetImage(rgb_image, sequence_reader.fx, sequence_reader.fy, sequence_reader.cx, sequence_reader.cy);
				visualizer.SetObservationLandmarks(face_model.detected_landmarks, face_model.detection_certainty, face_model.GetVisibilities());
				visualizer.SetObservationGaze(
					gaze0,
					gaze1,
					LandmarkDetector::CalculateAllEyeLandmarks(face_model),
					LandmarkDetector::Calculate3DEyeLandmarks(face_model, sequence_reader.fx, sequence_reader.fy, sequence_reader.cx, sequence_reader.cy),
					face_model.detection_certainty);
				visualizer.SetFps(fps_tracker.GetFPS());

				{
					cv::Mat vis = visualizer.GetVisImage();
					std::ostringstream ss;
					ss << "gaze_angle: (" << gaze_angle[0] << ", " << gaze_angle[1] << ") rad";
					cv::putText(vis, ss.str(), cv::Point(10, 45), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
				}

				char key = visualizer.ShowObservation();
				if (key == 'q' || key == 27)
				{
					return 0;
				}
				if (key == 'r')
				{
					face_model.Reset();
				}
			}

			++total_frame_counter;
			if (opts.max_frames > 0 && total_frame_counter >= opts.max_frames)
			{
				return 0;
			}

			rgb_image = sequence_reader.GetNextFrame();
		}

		face_model.Reset();
		sequence_reader.Close();
	}

	return 0;
}
