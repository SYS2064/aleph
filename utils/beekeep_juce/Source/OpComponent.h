/*
  ==============================================================================

    OpComponent.h
    Created: 10 Feb 2016 3:19:25am
    Author:  ezra

  ==============================================================================
*/

#ifndef OPCOMPONENT_H_INCLUDED
#define OPCOMPONENT_H_INCLUDED


#include "../JuceLibraryCode/JuceHeader.h"
#include "op.h"
#include "OpGraph.h"
#include "GraphEditorComponent.h"

class OpComponent : public Component
{
public:
    OpComponent (OpGraph* graph_, op_t* op, u16 idx);
    ~OpComponent();
    
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    
    // TODO: override thist to include pin children
    //bool hitTest (int x, int y) override;
    
    void paint (Graphics& g) override;
    void update(void);
    
    
    // row width and height specified as proportion of screen
    static const double ROW_W;
    static const double ROW_H;
    
    // get required component height for gicven op, as proportion of screen height
    static double getScreenHeight(op_t* op);
    static double getScreenWidth(void) { return ROW_W; }

    GraphEditorComponent* getGraphEditor() const noexcept
    {
        return findParentComponentOfClass<GraphEditorComponent>();
    }
    
    OpGraph* graph_;
    op_t* op_;
    u16 op_idx_;
    
    u8 num_ins_;
    u8 num_outs_;
    
    int pin_size_ ;
    Font font_ ;
    
    Point<int> pos0_;
    
};


#endif  // OPCOMPONENT_H_INCLUDED
