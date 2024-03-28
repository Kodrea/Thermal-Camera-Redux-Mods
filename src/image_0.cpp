#ifndef _IMAGE_0_CPP
#define _IMAGE_0_CPP

                TS( imageMicros = currentTimeMicros(); )

#if ! DRAW_SINGLE_THREAD // Moved to mainPrivate() for single thread builds
		pthread_mutex_lock( &lockAutoRangingMutex_image );
                imageFrame = Mat( *(threadData.rawFrame), imageFrameROI ).clone();
                if ( RotateDisplay ) { rotate( imageFrame, imageFrame, rotateFlags[ RotateDisplay ] ); }
		pthread_mutex_unlock( &lockAutoRangingMutex_image );
#endif

                if ( WINDOW_THERMAL != controls.windowFormat ) {

			if ( lockAutoRanging ) {
				if (	( ! threadData.inputFile ) && 
					(FILTER_TYPE_NONE != filterType) )
			       	{
					// Can't access thermalFrame until it is split and rotated
					pthread_mutex_lock( &lockAutoRangingMutex_therm );
					// Offline mode, selectable mapping filters
					lockAutoRangeFilter( thermalFrame, imageFrame );
					pthread_mutex_unlock( &lockAutoRangingMutex_therm );
				}
			}

                        cv::Mat edges;  // where's a better place to put this?
                        cv::Mat edgesColored;
                        int cannyFlag = 0;

                   if ( Use_Histogram ) {
                        cannyFlag = 1;
                        // Assuming histogramWrapper correctly populates 'edges' with the edge image
                        histogramWrapper(imageFrame, imageFrame, 0, edges);
                        //edge should be CV_8UC1

                        }


                        cvtColor ( imageFrame, rgbImageFrame, COLOR_YUV2BGR_YUYV, CVT_CHAN_FLAG );

                        // call canny edge detection somewhere else?
                        if (cannyFlag == 1){
                        // Overlay edgesColored onto rgbImageFrame
                        rgbImageFrame.setTo(cv::Scalar(0, 255, 0), edges); // Set pixels to green where edges are detected

                        }

                        // Set contrast
                        // void cv::convertScaleAbs (InputArray src, OutputArray dst, double alpha=1, double beta=0)
                        // dst(I)=saturate\_cast<uchar>(|src(I)∗alpha+beta|)
                        if ( 1.0 != controls.alpha ) {
                                // Optimization:  Only set alpha/contrast if it is NOT 1.0
                                convertScaleAbs(rgbImageFrame, rgbImageFrame, controls.alpha); // Contrast
                        }
                }

                ASSERT(( TC_WIDTH  == imageFrame.cols ));
                ASSERT(( TC_HEIGHT == imageFrame.rows ));

                TS( tmp = currentTimeMicros(); )

                TS( threadData.rotMicros   += ( tmp - imageMicros ); ) // track relative benchmarks
                TS( threadData.imageMicros += ( tmp - imageMicros ); ) // track relative benchmarks

#endif
