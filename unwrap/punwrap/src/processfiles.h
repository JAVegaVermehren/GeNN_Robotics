#pragma once

#include "unwrap.h"

/*
 * ffmpeg is used for copying the audio stream from the original to the
 * unwrapped video
 */
#ifndef FFMPEG_PATH
#define FFMPEG_PATH "/usr/bin/ffmpeg"
#endif

using namespace std;
using namespace cv;

/* breaks a filepath into path, name (with extension) and extension */
void getfileparts(string filepath, string &path, string &name)
{
    // find last slash
    size_t slash = filepath.rfind('/');
    if (slash == string::npos)
        slash = 0;
    else
        slash++;

    path = filepath.substr(0, slash);
    name = filepath.substr(slash);
}

/* unwrap a JPEG file */
void processjpeg(const char* filepath)
{
    string path, name;
    getfileparts(filepath, path, name);

    // read image into memory
    Mat im = imread(filepath, CV_LOAD_IMAGE_COLOR);

    // matrix to store unwrapped image
    Mat imunwrap(unwrap_height, unwrap_width, im.type());

    // matrices to store maps for unwrapping
    Mat map_x(unwrap_height, unwrap_width, CV_32FC1);
    Mat map_y(unwrap_height, unwrap_width, CV_32FC1);

    // perform unwrapping
    if (!processframe(filepath, imunwrap, im, map_x, map_y)) {
        return;
    }

    // save file
    string outfilename = path + "unwrapped_" + name;
    cout << "Saving image to " << outfilename << "..." << endl;
    vector<int> params;
    params.push_back(CV_IMWRITE_JPEG_QUALITY);
    params.push_back(100);
    imwrite(outfilename, imunwrap, params);
}

bool copysound = true;

/* unwrap an MP4 video */
void processmp4(const char* filepath)
{
    string path, name;
    getfileparts(filepath, path, name);

    // open video file
    VideoCapture cap(filepath);

    // read in first frame
    Mat fr;
    cap >> fr;

    // matrices to store maps for unwrapping
    Mat map_x(unwrap_height, unwrap_width, CV_32FC1);
    Mat map_y(unwrap_height, unwrap_width, CV_32FC1);

    // matrix to store unwrapped image
    Mat imunwrap(unwrap_height, unwrap_width, fr.type());

    // try unwrapping first frame and return if fail
    if (!processframe(filepath, imunwrap, fr, map_x, map_y)) {
        cap.release();
        return;
    }

    // final filename for unwrapped video
    string outfilename = path + "unwrapped_" + name;

    // temporary file name to which we write initially
    string tempfilename = copysound ? path + ".TEMP.MP4" : outfilename;

    // start writing to file
    cout << "Saving video to " << outfilename << "..." << endl;
    VideoWriter writer(tempfilename, 0x21, cap.get(CV_CAP_PROP_FPS), imunwrap.size());
    if (!writer.isOpened()) {
        cerr << "Error: Could not open file for writing" << endl;
        return;
    }

    // write first unwrapped frame
    writer.write(imunwrap);

    // unwrap successive frames and write to file
    for (;;) {
        cap >> fr;
        if (fr.empty()) {
            break;
        }

        unwrap(imunwrap, fr, map_x, map_y);
        writer.write(imunwrap);
    }

    // dispose of writer and reader when finished
    cap.release();
    writer.release();

    if (copysound) {
        // attempt to copy audio stream from original to new file with ffmpeg
        int stat = system((FFMPEG_PATH " -y -i \"" + tempfilename + "\" -i \"" +
                string(filepath) + "\" -map 0:v -map 1:a -c copy -shortest \"" +
                outfilename + "\" >/dev/null 2>&1").c_str());
        if (stat != 0) {
            cerr << "Error (" << stat << ") occurred while copying audio with ffmpeg" << endl;

            /*
             * just rename the temporary file to the output file, so there won't be
             * audio but at least there'll be video
             */
            rename(tempfilename.c_str(), outfilename.c_str());
        } else {
            // successfully copied audio, so delete temporary file
            remove(tempfilename.c_str());
        }
    }
}

