# Warehouse Manager

Tool for advanced simulation and optimization of warehouse.

---

## Dependencies

 * g++8                (c++17, structured bindings, filesystem, ...)
 * qt5                 (https://www.qt.io)
 * tinyxml2            (https://github.com/leethomason/tinyxml2, included)
 * simlib/c++          (http://www.fit.vutbr.cz/~peringer/SIMLIB/)
 * qcustomplot         (https://www.qcustomplot.com, included)
 * qdarkstylesheet     (https://github.com/ColinDuquesnoy/QDarkStyleSheet, included)

---

## Attributions

 * [QtEditableItems](https://github.com/fadyosman/QtEditableItems) - Used to enhance graphics items
 - [MAX-MIN-Ant-System](https://github.com/RSkinderowicz/MAX-MIN-Ant-System) - Two functions in pathfinder based on this project

---

## Usage

For more detailed description see application help.

**Warehouse Manager:**
  `./whm_gui`
---
**Generator:**
  `./whm_gen -o data/orders.xml -a data/articles.csv`
---
**Simulator (evaluate on test set of orders):**
  `./whm_sim -o data/orders_test.xml -i data/locations.csv -l data/layout.xml`
---
**Optimizer (train on train set of orders):**
  `./whm_opt -o data/orders_train.xml -a data/articles.csv -l data/layout.xml -i data/locations.csv -O 1-6`
---
**PathFinder (evaluate for one order):**
  `./whm_paf -o data/orders_test.xml -a data/articles.csv -l data/layout.xml -i data/locations.csv [-s]`
