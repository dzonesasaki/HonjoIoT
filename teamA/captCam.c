#include <cv.h>
#include <highgui.h>
#include <stdio.h>

const char gsVersion[]="v00_4_d";

#define NBUF 1024
int main (int argc, char *argv[]) 
{
	CvCapture *capture = 0;
	IplImage *frame = 0;
	double w = 640, h = 480;//change to VGA from QVGA by Dzone 120926_17:56 
	int c;
	unsigned int uilp;
	const char FileNameSv[]="captImage.bmp";
	char FNameSv[NBUF];
	unsigned int uiFlagOpenWin=0;
	unsigned int uiIndxCam=0;
	
	// usage a.exe [Num camera] [filename] [SwOpenWin]
	
//	if (argc == 1 || (argc == 2 && strlen (argv[1]) == 1 && isdigit (argv[1][0])))
	if (argc >1  && strlen (argv[1]) == 1 && isdigit (argv[1][0]))
	{
		//121002_23:01 , capture = cvCreateCameraCapture (argc == 2 ? argv[1][0] - '0' : 0);
		uiIndxCam = argv[1][0] - '0';
		capture = cvCreateCameraCapture ( uiIndxCam );
	}
	else
		// debug : it can select CamNum if argc > 2, and 2 digit ,by Dzone 121002_23:06 
		if (argc >1  && strlen (argv[1]) == 2 && isdigit (argv[1][0]) && isdigit (argv[1][1]))
		{
			uiIndxCam =(argv[1][1] - '0')+(argv[1][0] - '0')*10;
			capture = cvCreateCameraCapture ( uiIndxCam );
			// for debug, printf("set %d\n",uiIndxCam);
		}
		else
			capture = cvCreateCameraCapture (0);
			// it will be cared for filename on argument[2]

	//check came by Dzone 121002_22:43 
	if (capture == NULL)
	{
		printf("can not find Camera\n");
		return(255);
	}

	if (argc >2 )
		strcpy(FNameSv,argv[2]);
	else
		if  ((argc == 2) && (isdigit (argv[1][0])!=1))
			strcpy(FNameSv,argv[1]);
		else
		{
			strcpy(FNameSv,FileNameSv);
			printf("save to : %s\n",FNameSv);
		}

	if (argc >3 )
	{
			uiFlagOpenWin = (atoi(argv[3])==1);
			printf("Please push ESC for shutter\n");
	}


	
	cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_WIDTH, w);
	cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_HEIGHT, h);
	
	if(uiFlagOpenWin==1)
		cvNamedWindow ("Capture", CV_WINDOW_AUTOSIZE);
	
	for(uilp=0;uilp<10;uilp++){
		frame = cvQueryFrame (capture);
	}

	if(uiFlagOpenWin==1)
	{
		while (1) {
			frame = cvQueryFrame (capture);
			cvShowImage ("Capture", frame);
			c = cvWaitKey (2);
			if (c == '\x1b')
			break;
		}
		cvDestroyWindow ("Capture");
	}


	cvSaveImage(FNameSv, frame,0);
	//avoid error "too few arguments cvSaveImage 
	
	cvReleaseCapture (&capture);
	//cvDestroyWindow ("Capture");

  return 0;
}
