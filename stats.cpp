
#include "stats.h"

stats::stats(PNG & im){
	//Take care of origin
	HSLAPixel* origin = im.getPixel(0,0);
	vector<int> origHist(36, 0); //Vector of 36 zeros
	origHist[(int)origin->h / 10] = 1;
	hist[0][0] = origHist;
	sumHueX[0][0] = origin->s * cos(origin->h * PI / 180);
	sumHueY[0][0] = origin->s * sin(origin->h * PI / 180);
	sumLum[0][0] = origin->l;
	sumSat[0][0] = origin->s;

	//Take care of x=0 column
	for (unsigned int row = 0; row < im.height(); row++) {
		HSLAPixel* px = im.getPixel(0,row);
		vector<int> lst;
		for (int i = 1; i < 36; i++) 
				lst.push_back(hist[0][row-1][i]);
		lst[(int)px->h / 10]++;
		hist[0][row] = lst;
		sumHueX[0][row] = sumHueX[0][row-1] + px->s * cos(px->h * PI / 180);
		sumHueY[0][row] = sumHueY[0][row-1] + px->s * sin(px->h * PI / 180);
		sumLum[0][row] = sumLum[0][row-1] + px->l;
		sumSat[0][row] = sumSat[0][row-1] + px->s;
	} 

	//Take care of y=0 row
	for (unsigned int col = 0; col < im.width(); col++) {
		HSLAPixel* px = im.getPixel(0,col);
		vector<int> lst;
		for (int i = 1; i < 36; i++) 
				lst.push_back(hist[col-1][0][i]);
		lst[(int)px->h / 10]++;
		hist[col][0] = lst;
		sumHueX[col][0] = sumHueX[col-1][0] + px->s * cos(px->h * PI / 180);
		sumHueY[col][0] = sumHueY[col-1][0] + px->s * sin(px->h * PI / 180);
		sumLum[col][0] = sumLum[col-1][0] + px->l;
		sumSat[col][0] = sumSat[col-1][0] + px->s;
	}	

	//Do the rest of the pixels
	for (unsigned int row = 0; row < im.height(); row++) {
		for (unsigned int col = 0; col < im.width(); col++) {
			HSLAPixel* px = im.getPixel(col,row);
			vector<int> lst;
			for (int i = 1; i < 36; i++) 
				lst.push_back(hist[col][row-1][i] + hist[col-1][row][i] - hist[col-1][row-1][i]);
			lst[(int)px->h / 10]++;
			sumHueX[col][row] = sumHueX[col][row-1] + sumHueX[col-1][row] - sumHueX[col-1][row-1] + px->s * cos(px->h * PI / 180);
			sumHueY[col][row] = sumHueY[col][row-1] + sumHueY[col-1][row] - sumHueY[col-1][row-1] + px->s * sin(px->h * PI / 180);
			sumLum[col][row] =  sumLum[col][row-1] + sumLum[col-1][row] - sumLum[col-1][row-1] + px->l;
			sumSat[col][row] =  sumSat[col][row-1] + sumSat[col-1][row] - sumSat[col-1][row-1] + px->s;
		}
	}

}

//Fully compatible with rectangles that wrap around and do weird stuff
long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
	return abs(lr.first - ul.first + 1) * abs(lr.second - ul.second + 1);
}

//Fully compatible with rectangles that wrap around and do weird stuff
HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
	int ulx = ul.first;
	int uly = ul.second;
	int lrx = lr.first;
	int lry = lr.second;

	int width = hist.size();
	int height = hist[0].size();

	long area = rectArea(ul, lr);
	double avghx;
	double avghy;
	double avgs;
	double avgl;

	if (ulx > lrx and uly > lry) { //Rec wraps around top and sides
		double sumBotRCornerX = sumHueX[width-1][height-1] - sumHueX[ulx-1][height-1] - sumHueX[width-1][uly-1] + sumHueX[ulx-1][uly-1];
		double sumBotLCornerX = sumHueX[lrx][height-1] - sumHueX[lrx][uly-1];
		double sumTopRCornerX = sumHueX[width-1][lry] - sumHueX[ulx-1][lry];
		double sumTopLCornerX = sumHueX[lrx][lry];
		avghx = (sumBotRCornerX + sumTopLCornerX + sumBotLCornerX + sumTopRCornerX) / area;

		double sumBotRCornerY = sumHueY[width-1][height-1] - sumHueY[ulx-1][height-1] - sumHueY[width-1][uly-1] + sumHueY[ulx-1][uly-1];
		double sumBotLCornerY = sumHueY[lrx][height-1] - sumHueY[lrx][uly-1];
		double sumTopRCornerY = sumHueY[width-1][lry] - sumHueY[ulx-1][lry];
		double sumTopLCornerY = sumHueY[lrx][lry];
		avghy = (sumBotRCornerY + sumTopLCornerY + sumBotLCornerY + sumTopRCornerY) / area;

		double sumBotRCorners = sumSat[width-1][height-1] - sumSat[ulx-1][height-1] - sumSat[width-1][uly-1] + sumSat[ulx-1][uly-1];
		double sumBotLCorners = sumSat[lrx][height-1] - sumSat[lrx][uly-1];
		double sumTopRCorners = sumSat[width-1][lry] - sumSat[ulx-1][lry];
		double sumTopLCorners = sumSat[lrx][lry];
		avgs = (sumBotRCorners + sumTopLCorners + sumBotLCorners + sumTopRCorners) / area;

		double sumBotRCornerl = sumLum[width-1][height-1] - sumLum[ulx-1][height-1] - sumLum[width-1][uly-1] + sumLum[ulx-1][uly-1];
		double sumBotLCornerl = sumLum[lrx][height-1] - sumLum[lrx][uly-1];
		double sumTopRCornerl = sumLum[width-1][lry] - sumLum[ulx-1][lry];
		double sumTopLCornerl = sumLum[lrx][lry];
		avgl = (sumBotRCornerl + sumTopLCornerl + sumBotLCornerl + sumTopRCornerl) / area;

	} else if (ulx > lrx) { //Rec wraps around only the sides
		double sumLX; //hue x value, part of rectangle on left side
		double sumRX;
		double sumLY;  //hue y value, part of rectangle on left side
		double sumRY;
		double sumLL;  //luminance value, part of rectangle on left side
		double sumRL;
		double sumLS;  //saturation value, part of rectangle on left side
		double sumRS;

		if (uly == 0) { //Rectangle is at top edge
			sumLX = sumHueX[lrx][lry];
			sumLY = sumHueY[lrx][lry];
			sumLL = sumLum[lrx][lry];
			sumLS = sumSat[lrx][lry];

			sumRX = sumHueX[width-1][lry] - sumHueX[ulx-1][lry];
			sumRY = sumHueY[width-1][lry] - sumHueY[ulx-1][lry];
			sumRL = sumLum[width-1][lry] - sumLum[ulx-1][lry];
			sumRS = sumSat[width-1][lry] - sumSat[ulx-1][lry];
		} else {
			sumLX = sumHueX[lrx][lry] - sumHueX[lrx][uly-1];
			sumLY = sumHueY[lrx][lry] - sumHueY[lrx][uly-1];
			sumLL = sumLum[lrx][lry] - sumLum[lrx][uly-1];
			sumLS = sumSat[lrx][lry] - sumSat[lrx][uly-1];

			sumRX = sumHueX[width-1][lry] - sumHueX[ulx-1][lry] - sumHueX[width-1][uly-1] + sumHueX[ulx-1][uly-1];
			sumRY = sumHueY[width-1][lry] - sumHueY[ulx-1][lry] - sumHueY[width-1][uly-1] + sumHueY[ulx-1][uly-1];
			sumRL = sumLum[width-1][lry] - sumLum[ulx-1][lry] - sumLum[width-1][uly-1] + sumLum[ulx-1][uly-1];
			sumRS = sumSat[width-1][lry] - sumSat[ulx-1][lry] - sumSat[width-1][uly-1] + sumSat[ulx-1][uly-1];
		}
		avghx = (sumRX + sumLX) / area;
		avghy = (sumRY + sumLY) / area;
		avgl = (sumLL + sumRL) / area;
		avgs = (sumLS + sumRS) / area;

	} else if (uly > lry) { //Rectangle wraps around only the top
		double sumUX; //hue x value of part of rectangle on upper side of image
		double sumLX; //hue x value of part of rectangle on lower side of image
		double sumLY; 
		double sumUY;
		double sumUL; //luminance value of part of rectangle on upper side
		double sumLL;
		double sumUS; //saturation value of part of rectangle on upper side
		double sumLS;

		if (ulx == 0) { //Rectangle is at left edge
			sumLX = sumHueX[lrx][height-1] - sumHueX[lrx][uly-1];
			sumLY = sumHueY[lrx][height-1] - sumHueY[lrx][uly-1];
			sumLL = sumLum[lrx][height-1] - sumLum[lrx][uly-1];
			sumLS = sumSat[lrx][height-1] - sumSat[lrx][uly-1];

			sumUX = sumHueX[lrx][lry];
			sumUY = sumHueY[lrx][lry];
			sumUL = sumLum[lrx][lry];
			sumUS = sumSat[lrx][lry];
		} else {
			sumUX = sumHueX[lrx][lry] - sumHueX[ulx-1][lry];
			sumUY = sumHueY[lrx][lry] - sumHueY[ulx-1][lry];
			sumUL = sumLum[lrx][lry] - sumLum[ulx-1][lry];
			sumUS = sumSat[lrx][lry] - sumSat[ulx-1][lry];

			sumLX = sumHueX[lrx][height-1] - sumHueX[lrx][uly-1] - sumHueX[ulx-1][uly] + sumHueX[ulx-1][uly-1];
			sumLY = sumHueY[lrx][height-1] - sumHueY[lrx][uly-1] - sumHueY[ulx-1][uly] + sumHueY[ulx-1][uly-1];
			sumLL = sumLum[lrx][height-1] - sumLum[lrx][uly-1] - sumLum[ulx-1][uly] + sumLum[ulx-1][uly-1];
			sumLS = sumSat[lrx][height-1] - sumSat[lrx][uly-1] - sumSat[ulx-1][uly] + sumSat[ulx-1][uly-1];
		}
		avghx = (sumUX + sumLX) / area;
		avghy = (sumUY + sumLY) / area;
		avgl = (sumLL + sumUL) / area;
		avgs = (sumLS + sumUS) / area;
	} else { //Rectangle is fully contained
		if (ulx == 0 and uly == 0) {//Rectangle corner at origin
			avghx = sumHueX[lrx][lry] / area;
			avghy = sumHueY[lrx][lry] / area;
			avgl = sumLum[lrx][lry] / area;
			avgs = sumSat[lrx][lry] / area;
		} else if (ulx == 0) { //Rectangle corner at left edge of image
			avghx = (sumHueX[lrx][lry] - sumHueX[lrx][uly-1]) / area;
			avghy = (sumHueY[lrx][lry] - sumHueY[lrx][uly-1]) / area;
			avgs = (sumSat[lrx][lry] - sumSat[lrx][uly-1]) / area;
			avgl = (sumLum[lrx][lry] - sumLum[lrx][uly-1]) / area;
		} else if (uly == 0) { //Rectangle corner at top of image
			avghx = (sumHueX[lrx][lry] - sumHueX[ulx-1][lry]) / area;
			avghy = (sumHueY[lrx][lry] - sumHueY[ulx-1][lry]) / area;
			avgs = (sumSat[lrx][lry] - sumSat[ulx-1][lry]) / area;
			avgl = (sumLum[lrx][lry] - sumLum[ulx-1][lry]) / area;
		} else { //Rectangle floating in image
			avghx = (sumHueX[lrx][lry] - sumHueX[lrx][uly-1] - sumHueX[ulx-1][lry] + sumHueX[ulx][uly]) / area;
			avghy = (sumHueY[lrx][lry] - sumHueY[lrx][uly-1] - sumHueY[ulx-1][lry] + sumHueY[ulx][uly]) / area;
			avgs = (sumSat[lrx][lry] - sumSat[lrx][uly-1] - sumSat[ulx-1][lry] + sumSat[ulx][uly]) / area;
			avgl = (sumLum[lrx][lry] - sumLum[lrx][uly-1] - sumLum[ulx-1][lry] + sumLum[ulx][uly]) / area;
		}
	}

	double hue = atan2( avghy * (PI / 180), avghx );
	while ( hue < 0 ) hue += 360;

	return HSLAPixel(hue, avgs, avgl, 1.0);
}

//Fully compatible with rectangles that wrap around and do weird stuff
vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){
	vector<int> lst(36,0);
	int ulx = ul.first;
	int uly = ul.second;
	int lrx = lr.first;
	int lry = lr.second;

	int width = hist.size();
	int height = hist[0].size();

	if (ulx > lrx and uly > lry) { //Rec wraps around top and sides
		for (int i = 0; i < 36; i++)
			lst[i] = hist[width-1][height-1][i] - hist[ulx-1][height-1][i] - hist[width-1][uly-1][i] + hist[ulx-1][uly-1][i] + hist[lrx][height-1][i] - hist[lrx][uly-1][i]
					+ hist[width - 1][lry][i] - hist[ulx-1][lry][i] + hist[lrx][lry][i];

	} else if (ulx > lrx) { //Rec wraps around only the sides
		if (uly == 0) { //Rectangle is at top edge
			for (int i = 0; i < 36; i++)
				lst[i] = hist[lrx][lry][i] + hist[width-1][lry][i] - hist[ulx-1][lry][i];
		} else {
			for (int i = 0; i < 36; i++)
				lst[i] = hist[lrx][lry][i] - hist[lrx][uly-1][i] + hist[width-1][lry][i] - hist[ulx-1][lry][i] - hist[width-1][uly-1][i] + hist[ulx-1][uly-1][i];
		}

	} else if (uly > lry) { //Rectangle wraps around only the top
		if (ulx == 0) { //Rectangle is at left edge
			for (int i = 0; i < 36; i++)
				lst[i] = hist[lrx][lry][i] + hist[lrx][height-1][i] - hist[lrx][uly-1][i];
		} else {
			for (int i = 0; i < 36; i++)
				lst[i] = hist[lrx][lry][i] - hist[ulx-1][lry][i] + hist[lrx][height-1][i] - hist[lrx][uly-1][i] - hist[ulx-1][uly][i] + hist[ulx-1][uly-1][i];
		}

	} else { //Rectangle is fully contained
			if (ulx == 0 and uly == 0) //Rectangle corner at origin
				return hist[lrx][lry];
			else if (ulx == 0) { //Rectangle corner at left edge of image
				for (int i = 0; i < 36; i++)
					lst[i] = hist[lrx][lry][i] - hist[lrx][uly-1][i];
			} else if (uly == 0) { //Rectangle corner at top of image
				for (int i = 0; i < 36; i++)
					lst[i] = hist[lrx][lry][i] - hist[ulx-1][lry][i];
			} else {
				for (int i = 0; i < 36; i++) {
					lst[i] = hist[lrx][lry][i] - hist[lrx][uly-1][i] - hist[ulx-1][lry][i] + hist[ulx][uly][i];
				}
			}
	}

	return lst;
}

// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn, int area){

    double entropy = 0.0;

    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 ) 
            entropy -= ((double) distn[i]/(double) area) 
                                    * log2((double) distn[i]/(double) area);
    }

    return  entropy;
}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){
	double entropy = 0.0;
	vector<int> rectHist = buildHist(ul, lr);
	int numPx = rectArea(ul, lr);

	for (int i = 0; i < 36; i++) {
		if (rectHist[i] > 0)
			entropy -= ((double)rectHist[i] / (double)numPx) * log2((double)rectHist[i] / (double)numPx);
	}

	return entropy;
}
