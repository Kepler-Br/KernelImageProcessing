# Kernel image processing

Program with GUI that allows you to: 
* Process your images with matrix (aka kernel);
* Process PNG, JPG, BMP and so on;
* Customize passes, matrix size;
* Customize matrix contents;
* Save matrix to text file;
* Doing everything mentioned before using GUI;
* Choose number of threads(usually auto detected);
* Use your own matrix to process image.

Supported platforms: 
* Linux;
* Windows.

How to start working with this? 

`File->Open Image` to open image you want to process; 

`File->Save Image` to set target file where you want to save your processed image; 

Press `Do it!` button. 

Divider will divide every pixel by its value.

If divider set to 0, then it tries to calculate it automatically.

Offset parameter will sum to color of every pixel. 


Example image is from [Big Buck Bunny](https://peach.blender.org/); 
