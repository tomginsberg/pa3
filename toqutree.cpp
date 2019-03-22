
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
	:centre(ctr),dimension(dim),avg(a),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
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
	stats* stat = new stats(imIn);
	pair<int,int> centre = findSplit(dim, stat);

	//Create the root node
	root = new Node(centre, k, stat.getAvg(ul, lr));
	pair<int,int> ulRel(0,0);
	pair<int,int> lrRel(dim-1, dim-1);
	root->ul = ulRel;
	root->lr = lrRel;

	delete stat;
	stat = nullptr;

	//Make children
	int newDim = dim / 2;
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

	//Get 2^k x 2^k subimage centred in imIn
	PNG* rootIm = makePNG(ul,lr,&imIn);
	//Build new PNGs that are stiched together from the 4 sub nodes
	PNG* imNE = makePNG(neul, nelr, rootIm);
	PNG* imNW = makePNG(nwul, nwlr, rootIm);
	PNG* imSE = makePNG(seul, selr, rootIm);
	PNG* imSW = makePNG(swul, swlr, rootIm);

	delete rootIm;
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
	return size(root);
}
int toqutree::size(Node* node) {
	if (node == nullptr) return 0;
	return size(node->NE) + size(node->NW) + size(node->SE) + size(node->SW) + 1;
}


toqutree::Node * toqutree::buildTree(PNG * im, int k) {
	//base case:
	if (k == 0) {
		Node* node = new Node(pair<int,int>(0,0), 0, *im->getPixel(0,0));
		return node;
	}

	int dim = pow(2,k);
	int newDim = dim / 2;
	stats* stat = new stats(*im);

	pair<int,int> centre = findSplit(dim, stat);
	pair<int,int> ul((centre.first - newDim) % dim, (centre.second - newDim) % dim);
	pair<int,int> lr((centre.first + newDim - 1) % dim, (centre.second + newDim - 1) % dim);
	Node* node = new Node(centre, k, stat.getAvg(ul, lr));
	node->ul = ul;
	node->lr = lr;

	delete stat;
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
	node->NE = buildTree(imNE, k-1);
	delete imNE;
	imNE = nullptr;

	node->NW = buildTree(imNW, k-1);
	delete imNW;
	imNW = nullptr;

	node->SE = buildTree(imSE, k-1);
	delete imSE;
	imSE = nullptr;

	node->SW = buildTree(imSW, k-1);
	delete imSW;
	imSW = nullptr;

	return node;
}

PNG toqutree::render(){
// My algorithm for this problem included a helper function
// that was analogous to Find in a BST, but it navigated the 
// quadtree, instead.
	unsigned int size = static_cast<unsigned int>(pow(2,root->dimension));
	PNG im(size, size);
	render_helper(root, im);
	return im;
}

void toqutree::render_helper(Node* node, PNG & im) {
	//Check if it is a leaf - this is a complete tree so if one child is null, they all are
	if (node->NE == nullptr) {
		int width = abs(node->lr.first - node->ul.first + 1);
		int height = abs(node->lr.second - node->ul.second + 1);

		for (int i = node->ul.first; i < node->ul.first + width; i++) {
			int x = i % (int)pow(2, node->dimension);
			for (int j = node->ul.second; j < node->ul.second + height; j++) {
				int y = j % (int)pow(2, node->dimension);
				*im.getPixel(x, y) = node->avg;
			}
		}
	} else {
		render_helper(node->NE,im);
		render_helper(node->NW,im);
		render_helper(node->SE,im);
		render_helper(node->SW,im);
	}
}

void toqutree::prune(double tol){
	prune_helper(root,tol);
}

bool toqutree::prune_helper(Node* node, double tol) {
	if (node == nullptr or node->NE == nullptr) return false;
	HSLAPixel a = node->avg;
	//Before pruning it is safe to assume if a node has NE, then it also has the other 3 children
	bool pne = false;
	bool pnw = false;
	bool pse = false;
	bool psw = false;
	if (node->NE->NE != nullptr) {
		pne = prune_helper(node->NE, tol);
		pnw = prune_helper(node->NW, tol);
		pse = prune_helper(node->SE, tol);
		psw = prune_helper(node->SW, tol);
	} else { //The children are leaves
		//If the children (all of which are leaves) are all within tolerance, delete them all
		if ((node->NE->avg.h - a.h) < tol and (node->NW->avg.h - a.h) < tol and (node->SE->avg.h - a.h) < tol and (node->SW->avg.h - a.h) < tol) {
			delete node->NE;
			delete node->NW;
			delete node->SE;
			delete node->SW;

			node->NE = nullptr;
			node->NW = nullptr;
			node->SE = nullptr;
			node->SW = nullptr;
			return true;
		}
		return false;
	}

	//On the way back out, check if node's children are now leaves
	if (pne and pnw and pse and psw) {
		if ((node->NE->avg.h - a.h) < tol and (node->NW->avg.h - a.h) < tol and (node->SE->avg.h - a.h) < tol and (node->SW->avg.h - a.h) < tol) {
			delete node->NE;
			delete node->NW;
			delete node->SE;
			delete node->SW;

			node->NE = nullptr;
			node->NW = nullptr;
			node->SE = nullptr;
			node->SW = nullptr;
			return true;
		}
		return false;
	}
}

/* called by destructor and assignment operator*/
void toqutree::clear(Node * & node){
	if (node == nullptr) return;

	if (node->NE != nullptr)
		clear(node->NE);
	if (node->NW != nullptr)
		clear(node->NW);
	if (node->SE != nullptr)
		clear(node->SE);
	if (node->SW != nullptr)
		clear(node->SW);
	if (node->NE == nullptr and node->NW == nullptr and node->SE == nullptr and node->SW == nullptr) {
		delete node;
		node = nullptr;
	}
}

/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {
	this->root = new Node(other->centre, other->dimension, other->avg);
	this->root->NE = copy_helper(other->NE);
	this->root->NW = copy_helper(other->NW);
	this->root->SE = copy_helper(other->SE);
	this->root->SW = copy_helper(other->SW);
}

toqutree::Node* toqutree::copy_helper(Node* curr) {
	if (curr == nullptr) return nullptr;
	Node* node = new Node(curr->centre, curr->dimension, curr->avg);
	node->NE = copy_helper(curr->NE);
	node->NW = copy_helper(curr->NW);
	node->SE = copy_helper(curr->SE);
	node->SW = copy_helper(curr->SW);
	return node;
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
				centre.first = i;
				centre.second = j;
			}
		}
	}

	return centre;
}

//Since PNG only offers a crop function that keeps a rectangle in the top left corner of the image,
//Move all the pixels from the region of interest to the top left corner and then crop
PNG* toqutree::makePNG(pair<int,int> ul, pair<int,int> lr, PNG* im) {
	int width = im->width();
	int height = im->height();
	int subImWidth = abs(lr.first - ul.first + 1);
	int subImHeight = abs(lr.second - ul.second + 1);

	PNG* newIm = new PNG;
	*newIm = *im;
	int icount = 0;
	int jcount = 0;
	//Mod operators important here since region of interest may wrap around image boundaries
	for (int i = ul.first; i < ul.first + subImWidth; i++) {
		int col = i % width;
		for (int j = ul.second; j < ul.second + subImHeight; j++) {
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
