
#include "datavis/Surf3D.hpp"

#include <osg/Geometry>
#include <osg/LineWidth>

#include <fstream>
#include <sstream>
#include <iostream>


using namespace datavis;
using namespace osg;

Surf3D::Surf3D()
{

}

Surf3D::Surf3D(const std::string &filename, osg::Vec4 startColor, Vec4 finalColor, bool autoAssemble) :
    startColor(startColor),
    finalColor(finalColor)
{
    std::ifstream filestream;
    filestream.open(filename.c_str());

    if(!filestream.is_open())
    {
        std::cerr << "Could not open '" << filename << "' to generate a surface plot!" << std::endl;
        return;
    }

    std::string fileline;
    double value;
    std::vector<double> dataline;
    size_t line_count = 0;
    while(filestream.good())
    {
        ++line_count;
        dataline.clear();
        std::getline(filestream, fileline);

        std::stringstream datastream(fileline);

        size_t value_count = 0;
        while(datastream.good())
        {
            datastream >> value;
            dataline.push_back(value);
            ++value_count;
        }
        
        if(value_count>1)
            data.push_back(dataline);
    }
    
    filestream.close();

    x_scale = 1.0;
    y_scale = 0.5;
    z_scale = 0.1;
    lower_axis_scale = 1.2;
    upper_axis_scale = 1.6;
    
    getOrCreateStateSet()->setAttributeAndModes(new LineWidth(2));
    getOrCreateStateSet()->setMode(GL_BLEND, StateAttribute::ON);
    getOrCreateStateSet()->setRenderingHint(StateSet::TRANSPARENT_BIN);
    
    if(autoAssemble)
        assemble();
}

Surf3D::~Surf3D()
{
    
}

static Vec3 getNormal(const Vec3Array& verts, size_t i1, size_t i2, size_t i3)
{
    Vec3 normal = (verts[i2]-verts[i1]) ^ (verts[i3]-verts[i1]);
    normal.normalize();
    return normal;
}

Vec4 Surf3D::getColor(size_t layer)
{
    if(data.size()>1)
        return (finalColor-startColor)*layer/(data.size()-1) + startColor;
    else
        return startColor;
}

void Surf3D::assemble()
{
    if(data.size() < 2)
    {
        std::cerr << "Not enough layers to make a surface plot!" << std::endl;
        return;
    }

    size_t layersize = data[0].size();
    size_t pointcount = (layersize-1)/2;

    _geom = new Geometry;
    _axisGeom = new Geometry;
    
    ref_ptr<Vec3Array> tverts = new Vec3Array;
    _verts = new Vec3Array;
    _axisVerts = new Vec3Array;
    _normals = new Vec3Array;
    _colors = new Vec4Array;
    _axisColors = new Vec4Array;

    _faces = new DrawElementsUInt(PrimitiveSet::QUADS, 0);
    _axisLines = new DrawElementsUInt(PrimitiveSet::LINES, 0);
    _nbinds = new UIntArray;
    
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
        double z = z_scale*values[0];
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
    
    for(size_t i=0; i<data.size()-1; ++i)
    {
        _verts->push_back((*tverts)[p[i][0][0]]);
        _faces->push_back(_verts->size()-1);
        _colors->push_back(getColor(i));
        
        _verts->push_back((*tverts)[p[i][0][1]]);
        _faces->push_back(_verts->size()-1);
        _colors->push_back(getColor(i));
        
        _verts->push_back((*tverts)[p[i+1][0][1]]);
        _faces->push_back(_verts->size()-1);
        _colors->push_back(getColor(i+1));
        
        _verts->push_back((*tverts)[p[i+1][0][0]]);
        _faces->push_back(_verts->size()-1);
        _colors->push_back(getColor(i+1));
        
        _normals->push_back(getNormal(*tverts, p[i][0][0], p[i+1][0][1], p[i][0][1]));
        _nbinds->push_back(_normals->size()-1);
        _nbinds->push_back(_normals->size()-1);
        _nbinds->push_back(_normals->size()-1);
        _nbinds->push_back(_normals->size()-1);
        
        for(size_t j=0; j<y_count; ++j)
        {
            
            for(size_t k=0; k<2; ++k)
            {
                _verts->push_back((*tverts)[p[i][j][k]]);
                _faces->push_back(_verts->size()-1);
                _colors->push_back(getColor(i));
                
                _verts->push_back((*tverts)[p[i][j+1][k]]);
                _faces->push_back(_verts->size()-1);
                _colors->push_back(getColor(i));
                
                _verts->push_back((*tverts)[p[i+1][j+1][k]]);
                _faces->push_back(_verts->size()-1);
                _colors->push_back(getColor(i+1));
                
                _verts->push_back((*tverts)[p[i+1][j][k]]);
                _faces->push_back(_verts->size()-1);
                _colors->push_back(getColor(i+1));
                
                _normals->push_back(getNormal(*tverts, p[i][j][k], p[i+1][j+1][k], p[i][j+1][k]));
                _nbinds->push_back(_normals->size()-1);
                _nbinds->push_back(_normals->size()-1);
                _nbinds->push_back(_normals->size()-1);
                _nbinds->push_back(_normals->size()-1);
            }
        }
        
        _verts->push_back((*tverts)[p[i][y_count][0]]);
        _faces->push_back(_verts->size()-1);
        _colors->push_back(getColor(i));
        
        _verts->push_back((*tverts)[p[i][y_count][1]]);
        _faces->push_back(_verts->size()-1);
        _colors->push_back(getColor(i));
        
        _verts->push_back((*tverts)[p[i+1][y_count][1]]);
        _faces->push_back(_verts->size()-1);
        _colors->push_back(getColor(i+1));
        
        _verts->push_back((*tverts)[p[i+1][y_count][0]]);
        _faces->push_back(_verts->size()-1);
        _colors->push_back(getColor(i+1));
        
        _normals->push_back(getNormal(*tverts, p[i][y_count][0], p[i][y_count][1], p[i+1][y_count][1]));
        _nbinds->push_back(_normals->size()-1);
        _nbinds->push_back(_normals->size()-1);
        _nbinds->push_back(_normals->size()-1);
        _nbinds->push_back(_normals->size()-1);
    }
    

    for(size_t i=0; i<2; ++i)
    {
        size_t ii = i==0? 0 : data.size()-1;
        for(size_t j=0; j<y_count; ++j)
        {
            _verts->push_back((*tverts)[p[ii][j][0]]);
            _faces->push_back(_verts->size()-1);
            _colors->push_back(getColor(ii));
            
            _verts->push_back((*tverts)[p[ii][j][1]]);
            _faces->push_back(_verts->size()-1);
            _colors->push_back(getColor(ii));
            
            _verts->push_back((*tverts)[p[ii][j+1][1]]);
            _faces->push_back(_verts->size()-1);
            _colors->push_back(getColor(ii));
            
            _verts->push_back((*tverts)[p[ii][j+1][0]]);
            _faces->push_back(_verts->size()-1);
            _colors->push_back(getColor(ii));
            
            _normals->push_back(getNormal(*tverts, p[ii][j][0], p[ii][j+1][1], p[ii][j][1]));
            _nbinds->push_back(_normals->size()-1);
            _nbinds->push_back(_normals->size()-1);
            _nbinds->push_back(_normals->size()-1);
            _nbinds->push_back(_normals->size()-1);
        }
    }

    _geom->setVertexArray(_verts);
    _geom->addPrimitiveSet(_faces);
    
    _geom->setNormalArray(_normals);
    _geom->setNormalIndices(_nbinds);
    _geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
    
    _geom->setColorArray(_colors);
    _geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    
    
    lower = Vec3(bounds[0],bounds[2],bounds[4]);
    upper = Vec3(bounds[1],bounds[3],bounds[5]);
    
    mid = (lower+upper)/2;
    
    Vec3 lower_inflated = (lower-mid)*lower_axis_scale + mid; lower_inflated[2] = lower[2];
    Vec3 upper_inflated = (upper-mid)*upper_axis_scale + mid;
    Vec3 x_extents(upper_inflated[0],lower_inflated[1],lower_inflated[2]);
    Vec3 y_extents(lower_inflated[0],upper_inflated[1],lower_inflated[2]);
    Vec3 z_extents(lower_inflated[0],lower_inflated[1],upper_inflated[2]);
    
    _axisVerts->push_back(lower_inflated);
    _axisVerts->push_back(x_extents);
    _axisVerts->push_back(y_extents);
    _axisVerts->push_back(z_extents);
    
    _axisLines->push_back(0);
    _axisLines->push_back(1);
    _axisLines->push_back(0);
    _axisLines->push_back(2);
    _axisLines->push_back(0);
    _axisLines->push_back(3);
    
    _axisColors->push_back(Vec4(0,0,0,1));
    
    _axisGeom->setVertexArray(_axisVerts);
    _axisGeom->addPrimitiveSet(_axisLines);
    _axisGeom->setColorArray(_axisColors);
    _axisGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
    
    
    _x_text = new osgText::Text;
    _y_text = new osgText::Text;
    _z_text = new osgText::Text;

    std::string fontname = "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf";
    
    _x_text->setCharacterSize(0.005);
    _x_text->setFont(fontname);
    _x_text->setText("Pelvis X");
    _x_text->setAxisAlignment(osgText::Text::SCREEN);
    _x_text->setPosition(x_extents+Vec3(0,0,(upper_inflated[2]-lower_inflated[2])*0.035+lower_inflated[2]));
    _x_text->setColor(Vec4(0,0,0,1));
    addDrawable(_x_text);
    
    _y_text->setCharacterSize(0.005);
    _y_text->setFont(fontname);
    _y_text->setText("Pelvis Y");
    _y_text->setAxisAlignment(osgText::Text::SCREEN);
    _y_text->setPosition(y_extents+Vec3(0,0,(upper_inflated[2]-lower_inflated[2])*0.02+lower_inflated[2]));
    _y_text->setColor(Vec4(0,0,0,1));
    addDrawable(_y_text);
    
    _z_text->setCharacterSize(0.005);
    _z_text->setFont(fontname);
    _z_text->setText("TPD");
    _z_text->setAxisAlignment(osgText::Text::SCREEN);
    _z_text->setPosition(z_extents+Vec3(0,0,(upper_inflated[2]-lower_inflated[2])*0.02+lower_inflated[2]));
    _z_text->setColor(Vec4(0,0,0,1));
    addDrawable(_z_text);
    
    addDrawable(_geom);
    addDrawable(_axisGeom);
}













