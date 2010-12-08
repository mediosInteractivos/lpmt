class quad
{

public:
    quad()
    {
    }

    ofPoint corners[4];

    ofPoint src[4];
    ofPoint dst[4];
    //lets make a matrix for openGL
    //this will be the matrix that peforms the transformation
    GLfloat matrix[16];
    ofTrueTypeFont ttf;
    ofTrueTypeFont ttf2;
    // img and video stuff
    ofImage img;
    ofVideoPlayer video;

    ball balls[80];

    int borderColor;
    ofColor bgColor;
    ofColor imgColorize;
    ofColor videoColorize;
    ofColor camColorize;

    int bgAlpha;
    // camera stuff
    ofTexture camTexture;
    int camWidth;
    int camHeight;
    int videoWidth;
    int videoHeight;

    float camMultX;
    float camMultY;
    float imgMultX;
    float imgMultY;
    float videoMultX;
    float videoMultY;

    int quadNumber;

    bool initialized;
    bool isSetup;
    bool isOn;
    bool colorBg;
    bool camBg;
    bool imgBg;
    bool videoBg;
    bool videoSound;

    int bgImg;
    int bgVideo;
    int videoVolume;

    vector<string> images;
    vector<string> videos;

    string loadedImg;
    string loadedVideo;



    void setup(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, vector<string> &imgFiles, vector<string> &videoFiles)
    {

        //loads load in some truetype fonts
        ttf.loadFont("type/frabk.ttf", 22);
        ttf2.loadFont("type/frabk.ttf", 14);
        //lets load a test image too
        //img.loadImage("car.jpg");

        loadedImg = string("");
        loadedVideo = string("");

        quadNumber = 0;
        //this is just for our gui / mouse handles
        //this will end up being the destination quad we are warping too

        corners[0].x = x1;
        corners[0].y = y1;

        corners[1].x = x2;
        corners[1].y = y2;

        corners[2].x = x3;
        corners[2].y = y3;

        corners[3].x = x4;
        corners[3].y = y4;

        images = imgFiles;
        videos = videoFiles;

        borderColor = 0x666666;

        //lets setup some stupid particles
        for(int i = 0; i < 80; i++)
        {
            balls[i].setup(ofRandom(10, ofGetWidth() - 10), ofRandom(10, ofGetHeight()-10), ofRandom(5, 25));
            balls[i].vel.x = ofRandom(1.5, 2.8);
            balls[i].vel.y = ofRandom(1.5, 2.8);
        }

	    // sets default variables
        initialized = True;
        isSetup = True;
        isOn = True;
        colorBg = False;
        camBg = False;

	    camWidth = 320;
	    camHeight = 240;
	    camMultX = 1;
	    camMultY = 1;
	    camTexture.allocate(camWidth*4,camHeight*4, GL_RGB);

	    imgMultX = 1.0;
	    imgMultY = 1.0;

	    videoMultX = 1.0;
	    videoMultY = 1.0;

        videoVolume = 0;

        bgColor.r = 0;
	    bgColor.g = 0;
	    bgColor.b = 0;
	    bgColor.a = 0;

        imgColorize.r = 1;
	    imgColorize.g = 1;
	    imgColorize.b = 1;
	    imgColorize.a = 1;

        videoColorize.r = 1;
	    videoColorize.g = 1;
	    videoColorize.b = 1;
	    videoColorize.a = 1;

        camColorize.r = 1;
	    camColorize.g = 1;
	    camColorize.b = 1;
	    camColorize.a = 1;

    }

    void update()
    {
        if (isOn) {

        // loads an image if it has changed (and if its index in array is >1 as we have . and .. at the beginning)
        if (imgBg && (bgImg>1)) {
            string imgName = images[bgImg];
            if (imgName != loadedImg) {
                img.loadImage("img/"+imgName);
                loadedImg = imgName;
            }
        }

        // loads video
        if (videoBg && (bgVideo>1)) {
            string videoName = videos[bgVideo];
            if (videoName != loadedVideo) {
                if (video.bLoaded) { video.closeMovie(); }
                video.loadMovie("video/"+videoName);
                videoWidth = video.width;
                videoHeight = video.height;
                video.play();
                loadedVideo = videoName;
                }
            video.setVolume(videoVolume);
            video.idleMovie();
            }



       // temp stuff
        for(int i = 0; i < 80; i++)
        {
            balls[i].update(ofGetWidth(), ofGetHeight());
        }


        //we set matrix to the default - 0 translation
        //and 1.0 scale for x y z and w
        for(int i = 0; i < 16; i++)
        {
            if(i % 5 != 0) matrix[i] = 0.0;
            else matrix[i] = 1.0;
        }

        //we set the warp coordinates
        //source coordinates as the dimensions of our window
        src[0].x = 0;
        src[0].y = 0;
        src[1].x = ofGetWidth();
        src[1].y = 0;
        src[2].x = ofGetWidth();
        src[2].y = ofGetHeight();
        src[3].x = 0;
        src[3].y = ofGetHeight();

        //corners are in 0.0 - 1.0 range
        //so we scale up so that they are at the window's scale
        for(int i = 0; i < 4; i++)
        {
            dst[i].x = corners[i].x  * (float)ofGetWidth();
            dst[i].y = corners[i].y * (float) ofGetHeight();
        }

    }
    }

    void draw()
    {
    if (isOn) {
        // save actual GL coordinates
        ofPushMatrix();

        // find transformation matrix
        findHomography(src, dst, matrix);


        //finally lets multiply our matrix
        //wooooo hoooo!
        glMultMatrixf(matrix);


        // -- NOW LETS DRAW!!!!!!  -----

        // if a solid color background is set it draws it
        if (colorBg) {
            ofFill();
            ofEnableAlphaBlending();
            ofSetColor(bgColor.r * 255, bgColor.g * 255, bgColor.b * 255, bgColor.a * 255);
            ofRect(1, 1, ofGetWidth()-2, ofGetHeight()-2);
            ofDisableAlphaBlending();
            ofNoFill();
        }

        //if an image background is chosen it draws it
        if (imgBg) {
        ofSetColor(imgColorize.r * 255, imgColorize.g * 255, imgColorize.b * 255, imgColorize.a * 255);
        img.draw(0,0,img.getWidth()*imgMultX, img.getHeight()*imgMultY);
        }

        //if a video background is chosen it draws it
        if (videoBg) {
        ofSetColor(videoColorize.r * 255, videoColorize.g * 255, videoColorize.b * 255, videoColorize.a * 255);
        video.draw(0,0,videoWidth*videoMultX, videoHeight*videoMultY);
        }

        //lets draw a bounding box if we are in setup mode
        ofNoFill();
        if (isSetup)
        {
            ofSetColor(borderColor);
            ofRect(1, 1, ofGetWidth()-2, ofGetHeight()-2);
        }

	    // camera stuff
	    if (camBg) {
	    ofSetColor(camColorize.r * 255, camColorize.g * 255, camColorize.b * 255, camColorize.a * 255);
	    camTexture.draw(0,0,camWidth*camMultX,camHeight*camMultY);
	    }

        // TEMP STUFF - our particles
        ofEnableAlphaBlending();
        ofSetColor(255, 120, 0, 130);
        ofFill();
        for(int i = 0; i < 40; i++)balls[i].draw();
        ofDisableAlphaBlending();

        // writes quad number - KEEP IT AT LAST POSITION IN draw()
        if (isSetup) {
            ofSetColor(0x000000);
            ttf.drawString("quad n. "+ofToString(quadNumber), ofGetWidth()/2, ofGetHeight()/2);
            ofSetColor(0xFFFFFF);
            ttf.drawString("quad n. "+ofToString(quadNumber), (ofGetWidth()/2)-4, (ofGetHeight()/2)-4);
        }

        //some text
        //ofSetColor(0x000000);
        //ttf2.drawString("warps images nicely too!", 558, 533);
        //ofSetColor(0xFF6600);
        //ttf2.drawString("warps images nicely too!", 560, 530);

        // restore previous coordinates
        ofPopMatrix();
    }
    }



    void gaussian_elimination(float *input, int n)
    {
        // ported to c from pseudocode in
        // http://en.wikipedia.org/wiki/Gaussian_elimination

        float * A = input;
        int i = 0;
        int j = 0;
        int m = n-1;
        while (i < m && j < n)
        {
            // Find pivot in column j, starting in row i:
            int maxi = i;
            for(int k = i+1; k<m; k++)
            {
                if(fabs(A[k*n+j]) > fabs(A[maxi*n+j]))
                {
                    maxi = k;
                }
            }
            if (A[maxi*n+j] != 0)
            {
                //swap rows i and maxi, but do not change the value of i
                if(i!=maxi)
                    for(int k=0; k<n; k++)
                    {
                        float aux = A[i*n+k];
                        A[i*n+k]=A[maxi*n+k];
                        A[maxi*n+k]=aux;
                    }
                //Now A[i,j] will contain the old value of A[maxi,j].
                //divide each entry in row i by A[i,j]
                float A_ij=A[i*n+j];
                for(int k=0; k<n; k++)
                {
                    A[i*n+k]/=A_ij;
                }
                //Now A[i,j] will have the value 1.
                for(int u = i+1; u< m; u++)
                {
                    //subtract A[u,j] * row i from row u
                    float A_uj = A[u*n+j];
                    for(int k=0; k<n; k++)
                    {
                        A[u*n+k]-=A_uj*A[i*n+k];
                    }
                    //Now A[u,j] will be 0, since A[u,j] - A[i,j] * A[u,j] = A[u,j] - 1 * A[u,j] = 0.
                }

                i++;
            }
            j++;
        }

        //back substitution
        for(int i=m-2; i>=0; i--)
        {
            for(int j=i+1; j<n-1; j++)
            {
                A[i*n+m]-=A[i*n+j]*A[j*n+m];
                //A[i*n+j]=0;
            }
        }
    }

    void findHomography(ofPoint src[4], ofPoint dst[4], float homography[16])
    {

        // create the equation system to be solved
        //
        // from: Multiple View Geometry in Computer Vision 2ed
        //       Hartley R. and Zisserman A.
        //
        // x' = xH
        // where H is the homography: a 3 by 3 matrix
        // that transformed to inhomogeneous coordinates for each point
        // gives the following equations for each point:
        //
        // x' * (h31*x + h32*y + h33) = h11*x + h12*y + h13
        // y' * (h31*x + h32*y + h33) = h21*x + h22*y + h23
        //
        // as the homography is scale independent we can let h33 be 1 (indeed any of the terms)
        // so for 4 points we have 8 equations for 8 terms to solve: h11 - h32
        // after ordering the terms it gives the following matrix
        // that can be solved with gaussian elimination:

        float P[8][9]=
        {
            {-src[0].x, -src[0].y, -1,   0,   0,  0, src[0].x*dst[0].x, src[0].y*dst[0].x, -dst[0].x }, // h11
            {  0,   0,  0, -src[0].x, -src[0].y, -1, src[0].x*dst[0].y, src[0].y*dst[0].y, -dst[0].y }, // h12

            {-src[1].x, -src[1].y, -1,   0,   0,  0, src[1].x*dst[1].x, src[1].y*dst[1].x, -dst[1].x }, // h13
            {  0,   0,  0, -src[1].x, -src[1].y, -1, src[1].x*dst[1].y, src[1].y*dst[1].y, -dst[1].y }, // h21

            {-src[2].x, -src[2].y, -1,   0,   0,  0, src[2].x*dst[2].x, src[2].y*dst[2].x, -dst[2].x }, // h22
            {  0,   0,  0, -src[2].x, -src[2].y, -1, src[2].x*dst[2].y, src[2].y*dst[2].y, -dst[2].y }, // h23

            {-src[3].x, -src[3].y, -1,   0,   0,  0, src[3].x*dst[3].x, src[3].y*dst[3].x, -dst[3].x }, // h31
            {  0,   0,  0, -src[3].x, -src[3].y, -1, src[3].x*dst[3].y, src[3].y*dst[3].y, -dst[3].y }, // h32
        };

        gaussian_elimination(&P[0][0],9);

        // gaussian elimination gives the results of the equation system
        // in the last column of the original matrix.
        // opengl needs the transposed 4x4 matrix:
        float aux_H[]= { P[0][8],P[3][8],0,P[6][8], // h11  h21 0 h31
                         P[1][8],P[4][8],0,P[7][8], // h12  h22 0 h32
                         0      ,      0,0,0,       // 0    0   0 0
                         P[2][8],P[5][8],0,1
                       };      // h13  h23 0 h33

        for(int i=0; i<16; i++) homography[i] = aux_H[i];
    }








};

