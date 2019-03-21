
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
	stats* stat = new stat(imIn);

	pair<int,int> centre = findSplit(dim, stat);
	pair<int,int> ul((centre.first - dim / 2) % dim, (centre.second - dim / 2) % dim);
	pair<int,int> lr((centre.first + dim / 2 - 1) % dim, (centre.second + dim / 2 - 1) % dim);
	root = new Node(centre, k, stat.getAvg(ul, lr));

	delete stats;
	stat = nullptr;

	//Somehow build new PNGs that are stiched together from the 4 sub nodes
	PNG* imNE = new PNG(); //@TODO how to make new PNGs
	PNG* imNW = new PNG();
	PNG* imSE = new PNG();
	PNG* imSW = new PNG();

	//Build the subtrees
	root->NE = buildTree(&imNE, k-1);
	delete imNE;
	imNE = nullptr;

	root->NW = buildTree(&imNW, k-1);
	delete imNW;
	imNW = nullptr;

	root->SE = buildTree(&imSE, k-1);
	delete imSE;
	imSE = nullptr;

	root->SW = buildTree(&imSW, k-1);
	delete imSW;
	imSW = nullptr;

}

int toqutree::size() {

}


toqutree::Node * toqutree::buildTree(PNG * im, int k) {
	int dim = pow(2,k);
	stats* stat = new stat(imIn);

	pair<int,int> centre = findSplit(dim, stat);
	pair<int,int> ul((centre.first - dim / 2) % dim, (centre.second - dim / 2) % dim);
	pair<int,int> lr((centre.first + dim / 2 - 1) % dim, (centre.second + dim / 2 - 1) % dim);
	Node* node = new Node(centre, k, stat.getAvg(ul, lr));

	delete stats
	stat = nullptr;

	//Somehow build new PNGs that are stiched together from the 4 sub nodes
	PNG imNE();
	PNG imNW();
	PNG imSE();
	PNG imSW();

	//Build the subtrees
	root->NE = buildTree(&imNE, k-1);
	delete imNE;
	imNE = nullptr;

	root->NW = buildTree(&imNW, k-1);
	delete imNW;
	imNW = nullptr;

	root->SE = buildTree(&imSE, k-1);
	delete imSE;
	imSE = nullptr;

	root->SW = buildTree(&imSW, k-1);
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
