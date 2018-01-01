# Png-manipulation
C++ program used to copy, edit, and prune images

Shortened version of the spec:

A bitmap image is composed of many identically-sized square pixels arranged in a grid. If the number of pixels is 2^k by 2^k for some k, then one can build a quadtree with its root corresponding to the entire image, and with each leaf corresponding to a pixel. This provides us with an interesting way to store an image: put an RGBApixel into each leaf of an appropriate quadtree.

getPixel will retrieve the pixel (i.e. the color) of the square region within which the smaller query grid cell would lie. (That is, it will return the element of the nonexistent leaf’s deepest surviving ancestor.)

 clockwiseRotate rotates the Quadtree object’s underlying image clockwise by 90 degrees
 
 The prune function, given a tolerance value, prunes the Quadtree as extensively as possible. (Note, however, that we do not want the prune function to do an “iterative” prune. It is conceivable that by pruning some mid-level node nn, an ancestor pp of nn then becomes prunable, due to the fact that the prune changed the leaves descended from pp. Your prune function should evaluate the prunability of each node based on the presence of all nodes, and then delete the subtrees based at nodes deemed prunable.)
 
pruneSize takes a single argument, an integer tolerance. It returns an integer. This function is similar to prune; however, it does not actually prune the Quadtree; rather, it returns a count of the total number of leaves the Quadtree would have if it were pruned as in the prune function.
 
idealPrune takes a single argument, a positive integer numLeaves. It returns an integer. This function should calculate and return the minimum tolerance necessary to guarantee that upon pruning the tree, no more than numLeaves leaves remain in the Quadtree.
