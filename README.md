# Warehouse Manager

Tool for advanced simulation and optimization of warehouse.
This software was developed as a part of diploma thesis and is not intended for commercial use.

---

## Dependencies

 * g++8 (c++17, structured bindings, filesystem, ...)
 * [Qt5](https://www.qt.io)
 * [TinyXml2](https://github.com/leethomason/tinyxml2) (included)
 * [SIMLIB/C++](http://www.fit.vutbr.cz/~peringer/SIMLIB/)
 * [QCustomPlot](https://www.qcustomplot.com) (included)
 * [QDarkStyleSheet](https://github.com/ColinDuquesnoy/QDarkStyleSheet) (included)

---

## Attributions

 * [QtEditableItems](https://github.com/fadyosman/QtEditableItems) - Used to enhance graphics items
 * [MAX-MIN-Ant-System](https://github.com/RSkinderowicz/MAX-MIN-Ant-System) - Two functions in pathfinder based on this project

---

## Usage

For more detailed description see application help.

**Warehouse Manager**:

 - `./whm_gui`

**Generator**:

 - `./whm_gen -o data/orders.xml -a data/articles.csv`

**Simulator**:

 - `./whm_sim -o data/orders_test.xml -i data/locations.csv -l data/layout.xml`

**PathFinder**:

 - `./whm_paf -o data/orders_test.xml -i data/locations.csv -l data/layout.xml [-s]`

**Optimizer**:

 - `./whm_opt -o data/orders_train.xml -a data/articles.csv -l data/layout.xml -i data/locations.csv -O 1-6`
