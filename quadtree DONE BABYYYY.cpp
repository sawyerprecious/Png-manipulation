/**
 * @file quadtree.cpp
 * Quadtree class implementation.
 */

#include <cstddef>
#include <cstdlib>
#include <iostream>

using namespace std;

#include "quadtree.h"
#include "png.h"

// Quadtree
//   - parameters: none
//   - constructor for the Quadtree class; makes an empty tree
Quadtree::Quadtree() 
{
	root = new QuadtreeNode();
	res = 1;
}

// Quadtree
//   - parameters: PNG const & source - reference to a const PNG
//                    object, from which the Quadtree will be built
//                 int resolution - resolution of the portion of source
//                    from which this tree will be built
//   - constructor for the Quadtree class; creates a Quadtree representing
//        the resolution by resolution block in the upper-left corner of
//        source
Quadtree::Quadtree(PNG const& source, int setresolution)
{
	buildTree(source, setresolution);
}

// Quadtree
//   - parameters: Quadtree const & other - reference to a const Quadtree
//                    object, which the current Quadtree will be a copy of
//   - copy constructor for the Quadtree class
Quadtree::Quadtree(Quadtree const& other) 
{

	root = new QuadtreeNode();
	root->x = 0;
	root->y = 0;

	res = other.res;
	buildRowFromTree(root, res, other.root);
}

// Helper for the copy constructor
// If oq is a leaf, fill in the leaf on the copy constructor.
// Otherwise, recurse with all children.
//	- parameters: QuadtreeNode* q - the current node to be filled in
//					int setresolution - the desired resolution
//					QuadtreeNode* oq - the node in the already complete tree corresponding to q
void Quadtree::buildRowFromTree(QuadtreeNode* q, int setresolution, QuadtreeNode* oq)
{
	if (oq!=NULL)
	{	
		if (oq->nwChild==NULL &&
			oq->neChild==NULL &&
			oq->swChild==NULL &&
			oq->seChild==NULL)
		{
			q->element = oq->element;
		}else{

			int sr = setresolution/2;

			q->nwChild = new QuadtreeNode(oq->x, oq->y);
			q->neChild = new QuadtreeNode(oq->x + sr, oq->y);
			q->swChild = new QuadtreeNode(oq->x, oq->y + sr);
			q->seChild = new QuadtreeNode(oq->x + sr, oq->y + sr);

			QuadtreeNode* nwc = q->nwChild;
			QuadtreeNode* nec = q->neChild;
			QuadtreeNode* swc = q->swChild;
			QuadtreeNode* sec = q->seChild;

			buildRowFromTree(nwc, sr, oq->nwChild);
			buildRowFromTree(nec, sr, oq->neChild);
			buildRowFromTree(swc, sr, oq->swChild);
			buildRowFromTree(sec, sr, oq->seChild);


			q->element = oq->element;
		}
	}
}

// ~Quadtree
//   - parameters: none
//   - destructor for the Quadtree class
Quadtree::~Quadtree()
{
	destructorHelper(root);
}

// Helper for destructor.  Deletes all node's children, then the node.
void Quadtree::destructorHelper(QuadtreeNode* curr)
{
	if (curr != NULL)
	{
	destructorHelper(curr->nwChild);
	destructorHelper(curr->neChild);
	destructorHelper(curr->swChild);
	destructorHelper(curr->seChild);
	delete curr;
		curr = NULL;
	}else{
		return;
	}
}

// operator=
//   - parameters: Quadtree const & other - reference to a const Quadtree
//                    object, which the current Quadtree will be a copy of
//   - return value: a const reference to the current Quadtree
//   - assignment operator for the Quadtree class
Quadtree const& Quadtree::operator=(Quadtree const& other)
{
	root = new QuadtreeNode();
	root->x = 0;
	root->y = 0;

	res = other.res;
	buildRowFromTree(root, res, other.root);

	return other;
}

// builds a section of the tree.
// If the resolution requires q to be a leaf, fills in with proper color.
// Otherwise, recurses on all children.
void Quadtree::buildRow(QuadtreeNode* q, int setresolution, PNG const& source)
{
	if (setresolution == 1)
	{
		q->element = *source(q->x, q->y);
	}else{

		int sr = setresolution/2;

		q->nwChild = new QuadtreeNode(q->x, q->y);
		q->neChild = new QuadtreeNode(q->x + sr, q->y);
		q->swChild = new QuadtreeNode(q->x, q->y + sr);
		q->seChild = new QuadtreeNode(q->x + sr, q->y + sr);

		QuadtreeNode* nwc = q->nwChild;
		QuadtreeNode* nec = q->neChild;
		QuadtreeNode* swc = q->swChild;
		QuadtreeNode* sec = q->seChild;

		buildRow(nwc, sr, source);
		buildRow(nec, sr, source);
		buildRow(swc, sr, source);
		buildRow(sec, sr, source);

		q->element = avgChildren(q->nwChild, q->neChild, q->swChild, q->seChild);
	}
}

// Gets the average rgba value for a node's child nodes.
RGBAPixel Quadtree::avgChildren(QuadtreeNode* nw, QuadtreeNode* ne, QuadtreeNode* sw, QuadtreeNode* se){
	if (nw==NULL &&
		ne==NULL &&
		sw==NULL &&
		se==NULL)
	{
		return RGBAPixel();
	}
	int avred = (nw->element.red + ne->element.red + sw->element.red + se->element.red) /4;
	int avgreen = (nw->element.green + ne->element.green + sw->element.green + se->element.green) /4;
	int avblue = (nw->element.blue + ne->element.blue + sw->element.blue + se->element.blue) /4;
	int avalpha = (nw->element.alpha + ne->element.alpha + sw->element.alpha + se->element.alpha) /4;
	RGBAPixel* nrgba = new RGBAPixel(avred, avgreen, avblue, avalpha);
	return *nrgba;
}

// buildTree (public interface)
//   - parameters: PNG const & source - reference to a const PNG
//                    object, from which the Quadtree will be built
//                 int resolution - resolution of the portion of source
//                    from which this tree will be built
//   - transforms the current Quadtree into a Quadtree representing
//        the resolution by resolution block in the upper-left corner of
//        source
void Quadtree::buildTree(PNG const& source, int setresolution)
{
	root = new QuadtreeNode();
	res = setresolution;
	if (res <= 1)
	{
		//fill in leaves
		root->element = *source(0,0);
	}else{
		buildRow(root, setresolution, source);
	}
}

// getPixel (public interface)
//   - parameters: int x, int y - coordinates of the pixel to be retrieved
//   - return value: an RGBAPixel representing the desired pixel of the
//        underlying bitmap
//   - retrieves and returns the pixel at coordinates (x, y) in the
//        underlying bitmap
RGBAPixel Quadtree::getPixel(int x, int y) const
{
	return getPixelHelper(x, y, res, root, 0, 0);
}

// Helper for getPixel.  Uses resolution to determine whether desired pixel
// is in the nw, ne, sw, or se quadrants, and then recurses.
RGBAPixel Quadtree::getPixelHelper(int x, int y, int resolution, QuadtreeNode* curr, int cx, int cy) const
{
	if (resolution == 1)
	{
		return curr->element;
	}
	if (x < cx + resolution/2 && y < cy + resolution/2)
	{
		//in NW
		if (curr->nwChild == NULL)
		{
			return curr->element;
		}
		return getPixelHelper(x, y, resolution/2, curr->nwChild, cx, cy);
	}

		if (x >= cx + resolution/2 && y < cy + resolution/2)
	{
		//in NE
		if (curr->neChild == NULL)
		{
			return curr->element;
		}
		return getPixelHelper(x, y, resolution/2, curr->neChild, cx +resolution/2, cy);
	}

		if (x < cx +resolution/2 && y >= cy)
	{
		//in SW
		if (curr->swChild == NULL)
		{
			return curr->element;
		}
		return getPixelHelper(x, y, resolution/2, curr->swChild, cx, cy +resolution/2);
	}

		if (x >= cx +resolution/2 && y >= cy +resolution/2)
	{
		//in SE
		if (curr->seChild == NULL)
		{
			return curr->element;
		}
		return getPixelHelper(x, y, resolution/2, curr->seChild, cx +resolution/2, cy +resolution/2);
	}

	return RGBAPixel();
}

// Getter for resolution.
int Quadtree::getRes()
{
	return res;
}

// decompress (public interface)
//   - parameters: none
//   - return value: a PNG object representing this quadtree's underlying
//        bitmap
//   - constructs and returns this quadtree's underlying bitmap
PNG Quadtree::decompress() const
{
	RGBAPixel rgba = RGBAPixel();
	if (res == 1 && root->element == rgba)
	{
		PNG p = PNG();
		return p;
	}else{
		PNG rp = PNG(res,res);
		for (int x = 0; x < res; ++x)
		{
			for (int y = 0; y < res; ++y)
			{
				*rp(x,y) = getPixel(x,y);
			}
		}
		return rp;
	}
}

// clockwiseRotate (public interface)
//   - parameters: none
//   - transforms this quadtree into a quadtree representing the same
//        bitmap, rotated 90 degrees clockwise
void Quadtree::clockwiseRotate()
{
	rotateChildren(root);
}

// prune (public interface)
//   - parameters: int tolerance - an integer representing the maximum
//                    "distance" which we will permit between a node's color
//                    (i.e. the average of its descendant leaves' colors)
//                    and the color of each of that node's descendant leaves
//   - for each node in the quadtree, if the "distance" between the average
//        of that node's descendant leaves' colors and the color of each of
//        that node's descendant leaves is at most tolerance, this function
//        deletes the subtrees beneath that node; we will let the node's
//        color "stand in for" the colors of all (deleted) leaves beneath it
void Quadtree::prune(int tolerance)
{
	pruneHelper(tolerance, root);
}

// returns true colorDiff(orig, curr->children) < tolerance, false otherwise
void Quadtree::pruneHelper(int tolerance, QuadtreeNode* &curr)
{
	if (curr != NULL)
	{

		if (colorDiff(curr->nwChild, curr, tolerance) &&
			colorDiff(curr->neChild, curr, tolerance) &&
			colorDiff(curr->swChild, curr, tolerance) &&
			colorDiff(curr->seChild, curr, tolerance))
		{
			curr->nwChild = NULL;
			curr->neChild = NULL;
			curr->swChild = NULL;
			curr->seChild = NULL;
		}
		pruneHelper(tolerance, curr->nwChild);
		pruneHelper(tolerance, curr->neChild);
		pruneHelper(tolerance, curr->swChild);
		pruneHelper(tolerance, curr->seChild);

	}
}

// Returns true if the diff is <= the desired tolerance.  False otherwise.
bool Quadtree::colorDiff(QuadtreeNode* curr, QuadtreeNode* childof, int tolerance) const
{
	if (curr == NULL)
	{
		return false;
	}
	if (curr->nwChild == NULL)
	{	
		int diff = pow((curr->element.red - childof->element.red), 2) + pow((curr->element.green - childof->element.green), 2) + pow((curr->element.blue - childof->element.blue), 2);

		if (diff <= tolerance)
		{
			return true;
		}else{
			return false;
		}
	}else{
		return colorDiff(curr->nwChild, childof, tolerance) && colorDiff(curr->neChild, childof, tolerance) && colorDiff(curr->swChild, childof, tolerance) && colorDiff(curr->seChild, childof, tolerance);
	}

}

// pruneSize (public interface)
//   - parameters: int tolerance - an integer representing the maximum
//                    "distance" which we will permit between a node's color
//                    (i.e. the average of its descendant leaves' colors)
//                    and the color of each of that node's descendant leaves
//   - returns the number of leaves which this quadtree would contain if it
//        was pruned using the given tolerance; does not actually modify the
//        tree
int Quadtree::pruneSize(int tolerance) const
{
	return pruneSizeHelper(tolerance, root, root);
}

// Helper for pruneSize.  If all children are empty, returns 1.  Otherwise,
// calls the second helper to determine whether to prune the child nodes.
int Quadtree::pruneSizeHelper(int tolerance, QuadtreeNode* curr, QuadtreeNode* orig) const
{
	int numLeaves = 0;


	if (curr->nwChild !=NULL ||
		curr->swChild !=NULL ||
		curr->neChild !=NULL ||
		curr->seChild !=NULL)
	{

		if (pruneSizeHelper2(tolerance, curr->nwChild, orig) &&
			pruneSizeHelper2(tolerance, curr->neChild, orig) &&
			pruneSizeHelper2(tolerance, curr->swChild, orig) &&
			pruneSizeHelper2(tolerance, curr->seChild, orig))
		{
			return 1;
		}

		numLeaves = pruneSizeHelper(tolerance, curr->nwChild, curr->nwChild) + pruneSizeHelper(tolerance, curr->neChild, curr->neChild) + pruneSizeHelper(tolerance, curr->swChild, curr->swChild) + pruneSizeHelper(tolerance, curr->seChild, curr->seChild);


	}else{
		return 1;
	}

	return numLeaves;
}

// Another helper for pruneSize.  Takes a tolerance and recurses through all children to determine
// if all children are within tolerance.
bool Quadtree::pruneSizeHelper2(int tolerance, QuadtreeNode* curr, QuadtreeNode* orig) const
{
	if (curr->nwChild !=NULL ||
		curr->swChild !=NULL ||
		curr->neChild !=NULL ||
		curr->seChild !=NULL)
	{

		if (pruneSizeHelper2(tolerance, curr->nwChild, orig) &&
			pruneSizeHelper2(tolerance, curr->neChild, orig) &&
			pruneSizeHelper2(tolerance, curr->swChild, orig) &&
			pruneSizeHelper2(tolerance, curr->seChild, orig))
		{
			return true;
		}
		return false;

	}

	return colorDiff(curr, orig, tolerance);
}

// idealPrune (public interface)
//   - parameters: int numLeaves - an integer representing the number of
//                    leaves we wish the quadtree to have, after pruning
//   - returns the minimum tolerance such that pruning with that tolerance
//        would yield a tree with at most numLeaves leaves
int Quadtree::idealPrune(int numLeaves) const
{
	return idealPruneHelper(numLeaves, 255*255*3, 0);
}

// Helper for idealPrune.  Does a binary search to determine how many
// leaves would be present.
int Quadtree::idealPruneHelper(int numLeaves, int l, int r) const
{
        int mid = (l + r)/2;
 
        // If the element is present at the middle itself
        if (pruneSize(mid) == numLeaves)
        {
        	while(pruneSize(mid) == numLeaves)
        		mid--;
        	return mid + 1;
        }
 
        // If element is smaller than mid, then it can only be present
        // in left subarray
        if (pruneSize(mid) > numLeaves) return idealPruneHelper(numLeaves, l, mid-1);
 
        // Else the element can only be present in right subarray
        return idealPruneHelper(numLeaves, mid+1, r);
   }

// QuadtreeNode
//   - parameters: none
//   - constructor for the QuadtreeNode class; creates an empty
//        QuadtreeNode, with all child pointers NULL
Quadtree::QuadtreeNode::QuadtreeNode()
{
    neChild = seChild = nwChild = swChild = NULL;
    element = RGBAPixel();
    x = 0;
    y = 0;
}

// QuadtreeNode
//   - parameters: RGBAPixel const & elem - reference to a const
//        RGBAPixel which we want to store in this node
//   - constructor for the QuadtreeNode class; creates a QuadtreeNode
//        with element elem and all child pointers NULL
Quadtree::QuadtreeNode::QuadtreeNode(RGBAPixel const& elem)
{
    element = elem;
    neChild = seChild = nwChild = swChild = NULL;
    x = 0;
    y = 0;
}

// QuadtreeNode constructor with specified x and y.
Quadtree::QuadtreeNode::QuadtreeNode(int xc, int yc)
{
    element = RGBAPixel();
    neChild = seChild = nwChild = swChild = NULL;
    x = xc;
    y = yc;
}

// Rotates all of node's children
void Quadtree::rotateChildren(QuadtreeNode* q)
{
	if (q->nwChild != NULL ||
		q->neChild != NULL ||
		q->swChild != NULL ||
		q->seChild != NULL)
	{
		rotateChildren(q->nwChild);
		rotateChildren(q->neChild);
		rotateChildren(q->swChild);
		rotateChildren(q->seChild);

		QuadtreeNode* fnw = q->nwChild;
		QuadtreeNode* fne = q->neChild;
		QuadtreeNode* fse = q->seChild;
		QuadtreeNode* fsw = q->swChild;

		q->nwChild = fsw;
		q->neChild = fnw;
		q->swChild = fse;
		q->seChild = fne;
	}

}

