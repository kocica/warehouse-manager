# Warehouse Manager
# Filip Kocica (xkocic01@stud.fit.vutbr.cz)

Tool for advanced simulation and optimization of warehouse operations.
This software was developed as a part of diploma thesis and is not intended for commercial use.

Tool is provided with all required dependencies in the `src` directory, so it should be sufficient to only
run the following commands in project root directory:

    1. cd src
    2. chmod +x whm_*
    3. export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.

and execute binaries (./whm_<tool> <parameters>...). For whm_gui you may need X11 server (forwarding).

---

* Not needed if you only want to run the application, if you need to rebuild, you need to install dependencies *

In case you want to install the dependencies your self, here is how you can do so:
(also, if some of the libraries simlib/qt won't be available in the future, they are both enclosed in the `src/libs` directory)

    1. SIMLIB/C++
        a. Go to http://www.fit.vutbr.cz/~peringer/SIMLIB/source/
        b. Download version simlib-3.07
        c. Extract and move inside extracted directory
        d. Type `make` to compile sources
        e. Type `make install` to install library to /usr/local directory
        f. If linker cannot find simlib, you should set dynamic linker configuration for /usr/local/lib

    2. Qt5
        a. Go to https://download.qt.io/official_releases/qt/
        b. Navigate to some qt5 directory (optimal is version 5.9.5)
        c. Download *.run file of selected qt5 version
        d. Change permissions of the file (chmod +x ...) and run the file
        e. Go through the installer, qt5 should be installed on your machine
        f. Alternatively type into terminal: sudo apt-get install qt5-default

---

## Dependencies

 * g++ (recommended version >= 8.0 - c++17, structured bindings, filesystem - otherwise you may experience some warnings)
 * [Qt5](https://www.qt.io) (see installation above)
 * [TinyXml2](https://github.com/leethomason/tinyxml2) (included)
 * [SIMLIB/C++](http://www.fit.vutbr.cz/~peringer/SIMLIB/) (see installation above)
 * [QCustomPlot](https://www.qcustomplot.com) (included)
 * [QDarkStyleSheet](https://github.com/ColinDuquesnoy/QDarkStyleSheet) (included)

---

## Attributions

 * [QtEditableItems](https://github.com/fadyosman/QtEditableItems) - Used to enhance graphics items
 * [MAX-MIN-Ant-System](https://github.com/RSkinderowicz/MAX-MIN-Ant-System) - Two functions in pathfinder based on this project

---

## Usage

For more detailed description see application help or technical report.

First, navigate from root directory to `src/src` directory, where binaries together with dependencies are located.

Next, run tools following way:


**Warehouse Manager**:
Graphical implementation of editor for creation of warehouse abd all tools listed below.

 - `./whm_gui`

**Generator**:
Tool which is capable of generating two datasets of customer orders for optimization and testing.

 - `./whm_gen -o orders.xml -a data/150_200/articles.csv`

**Simulator**:
Tool used to simulate warehouse operations based on given warehouse model and orders.

 - `./whm_sim -o data/150_200/orders_test.xml -i data/150_200/locations.csv -l data/150_200/layout.xml`

**PathFinder**:
Tool is able to find the optimal path for given order or all orders in a given dataset.

 - `./whm_paf -o data/150_200/orders_test.xml -i data/150_200/locations.csv -l data/150_200/layout.xml [-s]`

**Optimizer**:
Tool which can optimize warehouse operations through finding the most fitting product allocation.
There are 6 optimizers in total, which one of them is providing random searching and one of them implements
Battista et al [1] approach. Others are implementations of evolutionary algorithms GA, DE, ABC and PSO redefined
to discrete space to solve SLAP (storage location assignment problem).

 - `./whm_opt -o data/150_200/orders_train.xml -a data/150_200/articles.csv -l data/150_200/layout.xml -i data/150_200/locations.csv -O 1-6`


[1] Battista, Claudia & Fumi, A. & Giordano, Francesco & Schiraldi, Massimiliano. (2011).
    Storage Location Assignment Problem: implementation in a warehouse design optimization tool.