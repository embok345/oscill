#ifndef PANE_H
#define PANE_H

#include "Child.h"
#include <algorithm>

class Pane : public Child {

  protected:

    std::vector<Child*> children;

  public:

    virtual ~Pane( ) override {
        std::vector<Child*>::iterator it = children.begin();
        while( it != children.end() ) {
            delete (*it);
            it++;
        }
    }

    virtual void add_child( Child *c ) {
        children.push_back( c );
    }
    virtual void remove_child( Child *c ) {
        children.erase(std::remove(children.begin(), children.end(), c), children.end());
    }
};

#endif
