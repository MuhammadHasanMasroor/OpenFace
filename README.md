# OpenFace (GazeOnly fork)

This is a trimmed, gaze-only C++ fork of the OpenFace repository. It keeps only:

- Landmark tracking (`lib/local/LandmarkDetector`)
- Gaze estimation (`lib/local/GazeAnalyser`)
- Utilities (camera/video IO + visualization) (`lib/local/Utilities`)
- A minimal executable that shows gaze in real time: `exe/GazeOnly` → `build/bin/GazeOnly`

## What you get

- Webcam / video gaze angles (radians) and 3D gaze vectors relative to the camera
- A realtime window with face landmarks + projected gaze rays (green)

Note: this is NOT “where on the screen you look”. For screen coordinates you need calibration (mapping gaze/head pose to screen points).

## Dependencies (Ubuntu / Debian)

Install:

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake \
  libopencv-dev libdlib-dev \
  libopenblas-dev liblapack-dev
```

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target GazeOnly -j
```

## Run

Webcam (press `q` to quit):

```bash
./build/bin/GazeOnly -device 0 -mloc build/bin/model/main_clnf_general.txt -show
```

Video:

```bash
./build/bin/GazeOnly -f /path/to/video.mp4 -mloc build/bin/model/main_clnf_general.txt -show
```

Try alternative CLNF models if tracking is unstable:

- `build/bin/model/main_clnf_wild.txt`
- `build/bin/model/main_clnf_multi_pie.txt`

## Citation

If you use any of the resources provided on this page in any of your publications we ask you to cite the following work and the work for a relevant submodule you used.

#### Overall system

**OpenFace 2.0: Facial Behavior Analysis Toolkit**
Tadas Baltrušaitis, Amir Zadeh, Yao Chong Lim, and Louis-Philippe Morency,
_IEEE International Conference on Automatic Face and Gesture Recognition_, 2018

#### Facial landmark detection and tracking

**Convolutional experts constrained local model for facial landmark detection**
A. Zadeh, T. Baltrušaitis, and Louis-Philippe Morency.
_Computer Vision and Pattern Recognition Workshops_, 2017

**Constrained Local Neural Fields for robust facial landmark detection in the wild**
Tadas Baltrušaitis, Peter Robinson, and Louis-Philippe Morency.
in IEEE Int. _Conference on Computer Vision Workshops, 300 Faces in-the-Wild Challenge_, 2013.

#### Eye gaze tracking

**Rendering of Eyes for Eye-Shape Registration and Gaze Estimation**
Erroll Wood, Tadas Baltrušaitis, Xucong Zhang, Yusuke Sugano, Peter Robinson, and Andreas Bulling
in _IEEE International Conference on Computer Vision (ICCV)_, 2015

#### Facial Action Unit detection

**Cross-dataset learning and person-specific normalisation for automatic Action Unit detection**
Tadas Baltrušaitis, Marwa Mahmoud, and Peter Robinson
in _Facial Expression Recognition and Analysis Challenge_,
_IEEE International Conference on Automatic Face and Gesture Recognition_, 2015

# Commercial license

For inquiries about the commercial licensing of the OpenFace toolkit please visit https://cmu.flintbox.com/#technologies/5c5e7fee-6a24-467b-bb5f-eb2f72119e59

# Final remarks

I did my best to make sure that the code runs out of the box but there are always issues and I would be grateful for your understanding that this is research code and a research project. If you encounter any problems/bugs/issues please contact me on github or by emailing me at tadyla@gmail.com for any bug reports/questions/suggestions. I prefer questions and bug reports on github as that provides visibility to others who might be encountering same issues or who have the same questions.

# Copyright

Copyright can be found in the Copyright.txt

You have to respect dlib, OpenBLAS, and OpenCV licenses.

Furthermore you have to respect the licenses of the datasets used for model training - https://github.com/TadasBaltrusaitis/OpenFace/wiki/Datasets
