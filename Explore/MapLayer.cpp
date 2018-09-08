//
//  MapLayer.cpp
//  GeoDa
//
//  Created by Xun Li on 8/24/18.
//

#include "MapLayer.hpp"

BackgroundMapLayer::BackgroundMapLayer()
:
pen_color(wxColour(192, 192, 192)),
brush_color(wxColour(255, 255, 255, 255)),
point_radius(2),
opacity(255),
pen_size(1),
show_boundary(false),
is_hide(true),
map_boundary(NULL)
{
}

BackgroundMapLayer::BackgroundMapLayer(wxString name, OGRLayerProxy* _layer_proxy, OGRSpatialReference* sr)
:
layer_name(name),
layer_proxy(_layer_proxy),
pen_color(wxColour(192, 192, 192)),
brush_color(wxColour(255, 255, 255, 255)),
point_radius(2),
opacity(255),
pen_size(1),
show_boundary(false),
is_hide(false),
map_boundary(NULL)
{
    shape_type = layer_proxy->GetGdaGeometries(shapes, sr);
    // this is for map boundary only
    shape_type = layer_proxy->GetOGRGeometries(geoms, sr);
    field_names = layer_proxy->GetIntegerFieldNames();
    for (int i=0; i<shapes.size(); i++) {
        highlight_flags.push_back(false);
    }
}

BackgroundMapLayer::~BackgroundMapLayer()
{
    if (map_boundary) {
        delete map_boundary;
    }
}

void BackgroundMapLayer::CleanMemory()
{
    // shapes and geoms will be not deleted until the map destroyed 
    for (int i=0; i<shapes.size(); i++) {
        delete shapes[i];
        delete geoms[i];
    }
}

void BackgroundMapLayer::SetHighlight(int idx)
{
    highlight_flags[idx] = true;
}

void BackgroundMapLayer::SetUnHighlight(int idx)
{
    highlight_flags[idx] = false;
}

void BackgroundMapLayer::SetName(wxString name)
{
    layer_name = name;
}

wxString BackgroundMapLayer::GetName()
{
    return layer_name;
}

BackgroundMapLayer* BackgroundMapLayer::Clone(bool clone_style)
{
    BackgroundMapLayer* copy =  new BackgroundMapLayer();
    copy->SetName(layer_name);
    copy->SetShapeType(shape_type);
    if (clone_style) {
        copy->SetPenColour(pen_color);
        copy->SetBrushColour(brush_color);
        copy->SetPointRadius(point_radius);
        copy->SetOpacity(opacity);
        copy->SetPenSize(pen_size);
        copy->SetShowBoundary(show_boundary);
        copy->SetHide(is_hide);
    }
    // deep copy
    copy->highlight_flags = highlight_flags;
    if (map_boundary) {
        copy->map_boundary = map_boundary->clone();
    }
    // not deep copy 
    copy->shapes = shapes;
    copy->geoms = geoms;
    copy->layer_proxy = layer_proxy;
    return copy;
}

int BackgroundMapLayer::GetNumRecords()
{
    return shapes.size();
}

bool BackgroundMapLayer::GetIntegerColumnData(wxString field_name, vector<wxInt64>& data)
{
    // this function is for finding IDs of multi-layer
    GdaConst::FieldType type = layer_proxy->GetFieldType(field_name);
    int col_idx = layer_proxy->GetFieldPos(field_name);
    if (type == GdaConst::long64_type) {
        for (int i=0; i<shapes.size(); ++i) {
            data[i] = (double)layer_proxy->data[i]->GetFieldAsInteger64(col_idx);
        }
        return true;
    }
    return false;
}

vector<wxString> BackgroundMapLayer::GetIntegerFieldNames()
{
    return field_names;
}

void BackgroundMapLayer::SetShapeType(Shapefile::ShapeType type)
{
    shape_type = type;
}

Shapefile::ShapeType BackgroundMapLayer::GetShapeType()
{
    return shape_type;
}

void BackgroundMapLayer::SetHide(bool flag)
{
    is_hide = flag;
}

bool BackgroundMapLayer::IsHide()
{
    return is_hide;
}

void BackgroundMapLayer::drawLegend(wxDC& dc, int x, int y, int w, int h)
{
    wxPen pen(pen_color);
    int r = brush_color.Red();
    int g = brush_color.Green();
    int b = brush_color.Blue();
    wxColour b_color(r,g,b,opacity);
    dc.SetPen(pen);
    dc.SetBrush(b_color);
    dc.DrawRectangle(x, y, w, h);
}

void BackgroundMapLayer::SetOpacity(int val)
{
    opacity = val;
}

int BackgroundMapLayer::GetOpacity()
{
    return opacity;
}

int BackgroundMapLayer::GetPenSize()
{
    return pen_size;
}

void BackgroundMapLayer::SetPenSize(int val)
{
    pen_size = val;
}

void BackgroundMapLayer::SetPenColour(wxColour &color)
{
    pen_color = color;
}

wxColour BackgroundMapLayer::GetPenColour()
{
    return pen_color;
}

void BackgroundMapLayer::SetBrushColour(wxColour &color)
{
    brush_color = color;
}

wxColour BackgroundMapLayer::GetBrushColour()
{
    return brush_color;
}

void BackgroundMapLayer::ShowBoundary(bool show)
{
    show_boundary = show;
    if (show) {
        if (map_boundary == NULL) {
            map_boundary = OGRLayerProxy::GetMapBoundary(geoms);
        }
    }
}

void BackgroundMapLayer::SetShowBoundary(bool flag)
{
    show_boundary = flag;
}

bool BackgroundMapLayer::IsShowBoundary()
{
    return show_boundary;
}

void BackgroundMapLayer::SetPointRadius(int radius)
{
    point_radius = radius;
}

int BackgroundMapLayer::GetPointRadius()
{
    return point_radius;
}

vector<GdaShape*>& BackgroundMapLayer::GetShapes()
{
    return shapes;
}







GdaShapeLayer::GdaShapeLayer(wxString _name, BackgroundMapLayer* _ml)
: name(_name), ml(_ml)
{
}

GdaShapeLayer::~GdaShapeLayer()
{
}

GdaShape* GdaShapeLayer::clone()
{
    // not implemented
    return NULL;
}

void GdaShapeLayer::Offset(double dx, double dy)
{
    
}

void GdaShapeLayer::Offset(int dx, int dy)
{
    
}

void GdaShapeLayer::applyScaleTrans(const GdaScaleTrans &A)
{
    if (ml->map_boundary) {
        ml->map_boundary->applyScaleTrans(A);
    } else {
        for (int i=0; i<ml->shapes.size(); i++) {
            ml->shapes[i]->applyScaleTrans(A);
        }
    }
}

void GdaShapeLayer::projectToBasemap(GDA::Basemap *basemap, double scale_factor)
{
    if (ml->map_boundary) {
        ml->map_boundary->projectToBasemap(basemap, scale_factor);
    } else {
        for (int i=0; i<ml->shapes.size(); i++) {
            ml->shapes[i]->projectToBasemap(basemap, scale_factor);
        }
    }
}

void GdaShapeLayer::paintSelf(wxDC &dc)
{
    if (ml->IsHide() == false) {
        wxPen pen(ml->GetPenColour(), ml->GetPenSize());
        if (ml->GetPenSize() == 0 ) {
            pen.SetColour(ml->GetBrushColour());
        }
        
        wxBrush brush(ml->GetBrushColour());
        
        if (ml->IsShowBoundary()) {
            ml->map_boundary->paintSelf(dc);
            return;
        }
        
        for (int i=0; i<ml->shapes.size(); i++) {
            if (ml->GetShapeType() == Shapefile::POINT_TYP) {
                GdaPoint* pt = (GdaPoint*)ml->shapes[i];
                pt->radius = ml->GetPointRadius();
            }
            ml->shapes[i]->setPen(pen);
            ml->shapes[i]->setBrush(brush);
            ml->shapes[i]->paintSelf(dc);
        }
    }
}

void GdaShapeLayer::paintSelf(wxGraphicsContext *gc)
{
    // not implemented (using wxGCDC instead)
}
