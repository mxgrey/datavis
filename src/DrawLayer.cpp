
#include "datavis/DrawLayer.hpp"


using namespace datavis;
using namespace osg;

#include <osg/Geometry>


#include <iostream>

DrawLayer::DrawLayer()
{
    
}

DrawLayer::DrawLayer(const std::string &filename, Vec4 startColor, Vec4 finalColor) :
    Surf3D(filename, startColor, finalColor, false)
{
    
}

static Vec3 getNormal(const Vec3Array& verts, size_t i1, size_t i2, size_t i3)
{
    Vec3 normal = (verts[i2]-verts[i1]) ^ (verts[i3]-verts[i1]);
    normal.normalize();
    return normal;
}

void DrawLayer::draw(std::vector<size_t> layers)
{
    size_t layersize = data[0].size();
    size_t pointcount = (layersize-1)/2;

    _axisGeom = new Geometry;
    
    ref_ptr<Vec3Array> tverts = new Vec3Array;
    _axisVerts = new Vec3Array;
    _axisColors = new Vec4Array;

    _axisLines = new DrawElementsUInt(PrimitiveSet::LINES, 0);
    
    std::vector<double> bounds; bounds.resize(6);
    for(size_t i=0; i<3; ++i)
    {
        bounds[2*i] = INFINITY;
        bounds[2*i+1] = -INFINITY;
    }
    
    for(size_t i=0; i<data.size(); ++i)
    {
        const std::vector<double>& values = data[i];
        std::vector< std::vector<unsigned int> > row;
        if(values.size() != layersize)
        {
            std::cerr << "Size of layer " << i+1 << " (" << values.size()
                      << ") does not match the rest (" << layersize <<  ")!"
                      << std::endl;
        }
        double z = 0;
        double x=0, y=0;

        std::vector<unsigned int> points;
        if(pointcount%2 != 0)
            std::cout << "Expected an even number of points per row, but got "
                      << pointcount << " for row #" << i << std::endl;

        for(size_t j=0; j<pointcount; ++j)
        {
            x = x_scale*values[2*j+1];
            y = y_scale*values[2*j+2];

            tverts->push_back(osg::Vec3(x,y,z));
            const Vec3& vert = tverts->back();
            
            for(size_t i=0; i<3; ++i)
            {
                if(vert[i] < bounds[2*i])
                    bounds[2*i] = vert[i];
                if(vert[i] > bounds[2*i+1])
                    bounds[2*i+1] = vert[i];
            }

            if(j%2==0)
            {
                points.clear();
                points.push_back(tverts->size()-1);
            }
            else
            {
                points.push_back(tverts->size()-1);
                row.push_back(points);
                points.clear();
            }
        }

        p.push_back(row);
    }
    
    
    size_t y_count = (pointcount)/2-1;
    
    for(size_t i=0; i<layers.size(); ++i)
    {
        _geom = new Geometry;
        _verts = new Vec3Array;
        _normals = new Vec3Array;
        _colors = new Vec4Array;
        _faces = new DrawElementsUInt(PrimitiveSet::QUADS, 0);
        _nbinds = new UIntArray;
        
        double z = 1e-5*i;

        size_t layer = layers[i];
        
        if(layer >= data.size())
        {
            std::cerr << "Cannot draw layer #" << layer << " max value is " 
                      << (int)(data.size())-1 << std::endl;
        }
        
        for(size_t j=0; j<y_count; ++j)
        {
            _verts->push_back((*tverts)[p[layer][j][0]]); _verts->back()[2] = z;
            _faces->push_back(_verts->size()-1);
            _colors->push_back(getColor(layer));
            
            _verts->push_back((*tverts)[p[layer][j][1]]); _verts->back()[2] = z;
            _faces->push_back(_verts->size()-1);
            _colors->push_back(getColor(layer));
            
            _verts->push_back((*tverts)[p[layer][j+1][1]]); _verts->back()[2] = z;
            _faces->push_back(_verts->size()-1);
            _colors->push_back(getColor(layer));
            
            _verts->push_back((*tverts)[p[layer][j+1][0]]); _verts->back()[2] = z;
            _faces->push_back(_verts->size()-1);
            _colors->push_back(getColor(layer));
            
            _normals->push_back(getNormal(*tverts, p[layer][j][0], p[layer][j+1][1], p[layer][j][1]));
            _nbinds->push_back(_normals->size()-1);
            _nbinds->push_back(_normals->size()-1);
            _nbinds->push_back(_normals->size()-1);
            _nbinds->push_back(_normals->size()-1);
        }
        
        _geom->setVertexArray(_verts);
        _geom->addPrimitiveSet(_faces);
        
        _geom->setNormalArray(_normals);
        _geom->setNormalIndices(_nbinds);
        _geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
        
        _geom->setColorArray(_colors);
        _geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        
        addDrawable(_geom);
        
        if(z>0)
        {
            _geom = new Geometry;
            _verts = new Vec3Array;
            _normals = new Vec3Array;
            _colors = new Vec4Array;
            _faces = new DrawElementsUInt(PrimitiveSet::QUADS, 0);
            _nbinds = new UIntArray;
            
            for(size_t j=0; j<y_count; ++j)
            {
                _verts->push_back((*tverts)[p[layer][j][0]]); _verts->back()[2] = -z;
                _faces->push_back(_verts->size()-1);
                _colors->push_back(getColor(layer)); _colors->back()[3] = 1;
                
                _verts->push_back((*tverts)[p[layer][j][1]]); _verts->back()[2] = -z;
                _faces->push_back(_verts->size()-1);
                _colors->push_back(getColor(layer)); _colors->back()[3] = 1;
                
                _verts->push_back((*tverts)[p[layer][j+1][1]]); _verts->back()[2] = -z;
                _faces->push_back(_verts->size()-1);
                _colors->push_back(getColor(layer)); _colors->back()[3] = 1;
                
                _verts->push_back((*tverts)[p[layer][j+1][0]]); _verts->back()[2] = -z;
                _faces->push_back(_verts->size()-1);
                _colors->push_back(getColor(layer)); _colors->back()[3] = 1;
                
                _normals->push_back(getNormal(*tverts, p[layer][j][0], p[layer][j+1][1], p[layer][j][1]));
                _nbinds->push_back(_normals->size()-1);
                _nbinds->push_back(_normals->size()-1);
                _nbinds->push_back(_normals->size()-1);
                _nbinds->push_back(_normals->size()-1);
            }
            
            _geom->setVertexArray(_verts);
            _geom->addPrimitiveSet(_faces);
            
            _geom->setNormalArray(_normals);
            _geom->setNormalIndices(_nbinds);
            _geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
            
            _geom->setColorArray(_colors);
            _geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
            
            addDrawable(_geom);
        }
    }
    
    
    lower = Vec3(bounds[0],bounds[2],bounds[4]);
    upper = Vec3(bounds[1],bounds[3],bounds[5]);
    
    mid = (lower+upper)/2;
    
    Vec3 lower_inflated = (lower-mid)*lower_axis_scale + mid;
    Vec3 upper_inflated = (upper-mid)*upper_axis_scale + mid;
    Vec3 x_extents(upper_inflated[0],lower_inflated[1],lower_inflated[2]);
    Vec3 y_extents(lower_inflated[0],upper_inflated[1],lower_inflated[2]);
    
    _axisVerts->push_back(lower_inflated);
    _axisVerts->push_back(x_extents);
    _axisVerts->push_back(y_extents);
    
    _axisLines->push_back(0);
    _axisLines->push_back(1);
    _axisLines->push_back(0);
    _axisLines->push_back(2);
    
    _axisColors->push_back(Vec4(0,0,0,1));
    
    _axisGeom->setVertexArray(_axisVerts);
    _axisGeom->addPrimitiveSet(_axisLines);
    _axisGeom->setColorArray(_axisColors);
    _axisGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
    
    
    _x_text = new osgText::Text;
    _y_text = new osgText::Text;

    std::string fontname = "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf";
    
    _x_text->setCharacterSize(0.005);
    _x_text->setFont(fontname);
    _x_text->setText("Pelvis X");
    _x_text->setAxisAlignment(osgText::Text::SCREEN);
    _x_text->setPosition(x_extents-Vec3(0,(upper_inflated[1]-lower_inflated[1])*0.025,0));
    _x_text->setColor(Vec4(0,0,0,1));
    addDrawable(_x_text);
    
    _y_text->setCharacterSize(0.005);
    _y_text->setFont(fontname);
    _y_text->setText("Pelvis Y");
    _y_text->setAxisAlignment(osgText::Text::SCREEN);
    _y_text->setPosition(y_extents-Vec3((upper_inflated[0]-lower_inflated[0])*0.025,0,0));
    _y_text->setColor(Vec4(0,0,0,1));
    addDrawable(_y_text);
    
    addDrawable(_axisGeom);
}

DrawLayer::~DrawLayer()
{
    
}
