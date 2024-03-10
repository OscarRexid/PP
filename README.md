
# Project Pipe

Attempt at making a fluid flow simulator, currently mostly a 2d graphics engine with placeable nodes.

Some future goals are interpretening between simplified cfd and classical node simulation. But first I want a function simulation software with placeable nodes and connecting pipes. 

## First tech demo has been released

### What does it include? 
Basic features to simulate water networks. Its really not a lot and I would say its more to be able to look back on in the future

### Supported platforms?
Only intended to work on and tested windows 10 and 11 but might work on others, untested.

### Whats next?
 - Acounting for resitances with geometry(Bends etc)
 - Boosters(Basic pumps)
 - Deleting items
 - Potentially the start to moddable fluids
 - Tanks of fluid
 - Better results post-processing
 - Some performance improvements

### More long term goals
 - Saving and loading
 - Modular fluids and parts with an editor
 - Temperature and heat transfer
 - CFD
 - Lots of performance optimization including but not limited to multithreading, gpu-accelleration and much more.
 - Better UI
 - Moving items and potentially even getting rid of the grid entirely or at least making grid snapping optional
 - Much Much More to come...

## Incomplete!

Warning if you for some reasson stumble over this project and this warning is still here it means the project is severly unfinished and not at all ready, you can still look at it and download it but dont expect a working product of any kind. It is as of right now mostly just a tech demo/prototype.

## License

[MIT](https://choosealicense.com/licenses/mit/)


## Acknowledgements

 ### Software
  - [SFML](https://www.sfml-dev.org/)
  - [ImGui](https://github.com/ocornut/imgui)
  - [ImGui SFML Backend](https://github.com/SFML/imgui-sfml)
  - [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)

 ### Literature 
  - [Finite-Element Method for Water-Distribution Networks (1975)](https://www.jstor.org/stable/41267962)
  - [LiquiNet Documentation](https://github.com/samadritakarmakar/LiquiNet/blob/master/Documentation/Documentation.pdf)
  - [Precise Explicit Approximations of the Colebrook-White Equation for Engineering Systems (2020)](https://link.springer.com/chapter/10.1007/978-3-030-57340-9_37)
