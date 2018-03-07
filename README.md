# Match-Template

Template Matching is a method for searching and finding the location of a template image in a larger image.
The template matching is based on the Euclidean distance traversed every pixel of the matched graph and then 
calculates the Euclidean distance between the pixels centered on the pixel and the overlapping part of the template image.
When the template graph grows larger, the calculation increases sharply, so I've used SSE optimizations.

![](screenshot.png)
