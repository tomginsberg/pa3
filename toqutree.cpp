
/**
 *
 * toqutree (pa3)
 * significant modification of a quadtree .
 * toqutree.cpp
 * This file will be used for grading.
 *
 */

#include "toqutree.h"

toqutree::Node::Node(pair<int,int> ctr, int dim, HSLAPixel a)
	:center(ctr),dimension(dim),avg(a),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
	{}

toqutree::~toqutree(){
	clear(root);
}

toqutree::toqutree(const toqutree & other) {
	root = copy(other.root);
}


toqutree & toqutree::operator=(const toqutree & rhs){
	if (this != &rhs) {
		clear(root);
		root = copy(rhs.root);
	}
	return *this;
}

toqutree::toqutree(PNG & imIn, int k){ 
	/* This constructor grabs the 2^k x 2^k sub-image centered */
	/* in imIn and uses it to build a quadtree. It may assume  */
	/* that imIn is large enough to contain an image of that size. */
	int dim = pow(2,k);

	//Find root node's corners - simply centred in image
	int centrex = imIn.width() / 2;
	int centrey = imIn.height() / 2;
	pair<int,int> ul(centrex - dim / 2, centrey - dim / 2);
	pair<int,int> lr(centrex + dim / 2 - 1, centrey + dim / 2 - 1);

	//Find root node's optimal splitting point
	stats* stat = new stat(imIn);
	pair<int,int> centre = findSplit(dim, stat);

	//Create the root node
	root = new Node(centre, k, stat.getAvg(ul, lr));

	delete stats;
	stat = nullptr;

	//Get 2^k x 2^k subimage centred in imIn
	PNG* rootIm = makePNG(ul,lr,imIn);
	//Build new PNGs that are stiched together from the 4 sub nodes
	PNG* imNE = makePNG(neul, nelr, rootIm);
	PNG* imNW = makePNG(nwul, nwlr, rootIm);
	PNG* imSE = makePNG(seul, selr, rootIm);
	PNG* imSW = makePNG(swul, swlr, rootIm);

	delete rootIml;
	rootIm = nullptr;

	//Build the subtrees
	root->NE = buildTree(imNE, k);
	delete imNE;
	imNE = nullptr;

	root->NW = buildTree(imNW, k);
	delete imNW;
	imNW = nullptr;

	root->SE = buildTree(imSE, k);
	delete imSE;
	imSE = nullptr;

	root->SW = buildTree(imSW, k);
	delete imSW;
	imSW = nullptr;
}

int toqutree::size() {

}


toqutree::Node * toqutree::buildTree(PNG * im, int k) {
	int dim = pow(2,k);
	int newDim = dim / 2;
	stats* stat = new stat(im);

	pair<int,int> centre = findSplit(dim, stat);
	pair<int,int> ul((centre.first - newDim) % dim, (centre.second - newDim) % dim);
	pair<int,int> lr((centre.first + newDim - 1) % dim, (centre.second + newDim - 1) % dim);
	Node* node = new Node(centre, k, stat.getAvg(ul, lr));

	delete stats
	stat = nullptr;

	//Find corners of all 4 child nodes
	int i = centre.first;
	int j = centre.second;
	pair<int, int> seul(i,j);
	pair<int,int> selr((i+newDim-1)%dim,(j+newDim-1)%dim);

	pair<int,int> neul(i,(j+newDim)%dim);
	pair<int,int> nelr((i+newDim-1)%dim,(j+dim-1)%dim);

	pair<int,int> swul((i+newDim)%dim,j);
	pair<int,int> swlr((i+dim-1)%dim,(j+newDim-1)%dim);

	pair<int,int> nwul((i+newDim)%dim,(j+newDim)%dim);
	pair<int,int> nwlr((i+dim-1)%dim,(j+dim-1)%dim);

	//Build new PNGs that are stiched together from the 4 child nodes
	PNG* imNE = makePNG(neul, nelr, im);
	PNG* imNW = makePNG(nwul, nwlr, im);
	PNG* imSE = makePNG(seul, selr, im);
	PNG* imSW = makePNG(swul, swlr, im);

	//Build the subtrees
	node->NE = buildTree(&imNE, k-1);
	delete imNE;
	imNE = nullptr;

	node->NW = buildTree(&imNW, k-1);
	delete imNW;
	imNW = nullptr;

	node->SE = buildTree(&imSE, k-1);
	delete imSE;
	imSE = nullptr;

	node->SW = buildTree(&imSW, k-1);
	delete imSW;
	imSW = nullptr;

	return node;
}

PNG toqutree::render(){

// My algorithm for this problem included a helper function
// that was analogous to Find in a BST, but it navigated the 
// quadtree, instead.

/* your code here */

}

/* oops, i left the implementation of this one in the file! */
void toqutree::prune(double tol){

	prune(root,tol);

}

/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){
/* your code here */
}

/* done */
/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {

}

pair<int,int> toqutree::findSplit(int dim, stats* stat) {
	int newDim = dim / 2;
	double minEntropy = 10000;
	pair<int,int> centre;

	for (int i = newDim / 2; i < newDim / 2 + newDim - 1; i++) {
		for (int j = newDim / 2; j < newDim / 2 + newDim - 1; j++) {
			pair<int, int> seul(i,j);
			pair<int,int> selr((i+newDim-1)%dim,(j+newDim-1)%dim);

			pair<int,int> neul(i,(j+newDim)%dim);
			pair<int,int> nelr((i+newDim-1)%dim,(j+2*newDim-1)%dim);

			pair<int,int> swul((i+newDim)%dim,j);
			pair<int,int> swlr((i+2*newDim-1)%dim,(j+newDim-1)%dim);

			pair<int,int> nwul((i+newDim)%dim,(j+newDim)%dim);
			pair<int,int> nwlr((i+2*newDim-1)%dim,(j+2*newDim-1)%dim);

			double avgEntropy = (stat->entropy(seul, selr) + stat->entropy(neul, nelr) + stat->entropy(swul, swlr) + stat->entropy(nwul, nwlr)) / 4.0;

			if (avgEntropy < minEntropy) {
				minEntropy = avgEntropy;
				centre(i,j);
			}
		}
	}

	return centre;
}

//Since PNG only offers a crop function that keeps a rectangle in the top left corner of the image,
//Move all the pixels from the region of interest to the top left corner and then crop
PNG* toqutree::makePNG(pair<int,int> ul, pair<int,int> lr, PNG* im) {
	int width = im.width();
	int height = im.height();
	int subImWidth = abs(lr.first - ul.first + 1);
	int subImHeight = abs(lr.second - ul.second + 1);

	PNG* newIm = new PNG;
	newIm = im;
	int icount = 0;
	int jcount = 0;
	//Mod operators important here since region of interest may wrap around image boundaries
	for (int i = ul.first; i < ul.first + subImWidth - 1; i++) {
		int col = i % width;
		for (int j = ul.second; j < ul.second + subImHeight - 1; j++) {
			int row = j % height;
			HSLAPixel * newPos = newIm->getPixel(icount, jcount);
			HSLAPixel * oldPos = im->getPixel(col, row);

			//Replace pixel in top left quadrant with pixel from within chosen rectangle of the image
			*newPos = *oldPos;

			jcount++;
		}
		icount++;
	}

	//Now that required section of image has been transferred to the top left quadrant, crop the image
	newIm->resize(subImWidth, subImHeight);
	return newIm;
}
