
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

Surf3D::Surf3D(const std::string &filename, osg::Vec4 startColor, Vec4 finalColor) :
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
    
    getOrCreateStateSet()->setAttributeAndModes(new LineWidth(5));
    getOrCreateStateSet()->setMode(GL_BLEND, StateAttribute::ON);
    getOrCreateStateSet()->setRenderingHint(StateSet::TRANSPARENT_BIN);
    
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
    return (finalColor-startColor)*layer/data.size() + startColor;
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
    _lineGeom = new Geometry;
    
    ref_ptr<Vec3Array> tverts = new Vec3Array;
    _verts = new Vec3Array;
    _normals = new Vec3Array;
    _colors = new Vec4Array;
    _lineColors = new Vec4Array;

    _faces = new DrawElementsUInt(PrimitiveSet::QUADS, 0);
    _lines = new DrawElementsUInt(PrimitiveSet::LINES, 0);
    _nbinds = new UIntArray;
    
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
//        _lineColors->push_back(lineColor);
//        _lines->push_back(_verts->size()-1);
//        size_t closer = _verts->size()-1;
        
        _verts->push_back((*tverts)[p[i][0][1]]);
        _faces->push_back(_verts->size()-1);
        _colors->push_back(getColor(i));
//        _lineColors->push_back(lineColor);
//        _lines->push_back(_verts->size()-1);
//        _lines->push_back(_verts->size()-1);
        
        _verts->push_back((*tverts)[p[i+1][0][1]]);
        _faces->push_back(_verts->size()-1);
        _colors->push_back(getColor(i+1));
//        _lineColors->push_back(lineColor);
//        _lines->push_back(_verts->size()-1);
//        _lines->push_back(_verts->size()-1);
        
        _verts->push_back((*tverts)[p[i+1][0][0]]);
        _faces->push_back(_verts->size()-1);
        _colors->push_back(getColor(i+1));
//        _lineColors->push_back(lineColor);
//        _lines->push_back(_verts->size()-1);
//        _lines->push_back(closer);
        
        _normals->push_back(getNormal(*tverts, p[i][0][0], p[i+1][0][1], p[i][0][1]));
        _nbinds->push_back(_normals->size()-1);
        _nbinds->push_back(_normals->size()-1);
        _nbinds->push_back(_normals->size()-1);
        _nbinds->push_back(_normals->size()-1);
        
//        _lines->push_back(p[i][0][0]);
//        _lines->push_back(p[i][0][1]);      //
//        _lines->push_back(p[i][0][1]);
//        _lines->push_back(p[i+1][0][1]);    //
//        _lines->push_back(p[i+1][0][1]);
//        _lines->push_back(p[i+1][0][0]);    //
//        _lines->push_back(p[i+1][0][0]);
//        _lines->push_back(p[i][0][0]);      //
        
//        for(size_t j=0; j<(pointcount-1)/2; ++j)
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
                
//                _lines->push_back(p[i][j][k]);
//                _lines->push_back(p[i][j+1][k]);        //
//                _lines->push_back(p[i][j+1][k]);
//                _lines->push_back(p[i+1][j+1][k]);      //
//                _lines->push_back(p[i+1][j+1][k]);
//                _lines->push_back(p[i+1][j][k]);        //
//                _lines->push_back(p[i+1][j][k]);
//                _lines->push_back(p[i][j][k]);          //
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
        
        
        
//        _lines->push_back(p[i][y_count][0]);
//        _lines->push_back(p[i][y_count][1]);            //
//        _lines->push_back(p[i][y_count][1]);
//        _lines->push_back(p[i+1][y_count][1]);          //
//        _lines->push_back(p[i+1][y_count][1]);
//        _lines->push_back(p[i+1][y_count][0]);          //
//        _lines->push_back(p[i+1][y_count][0]);
//        _lines->push_back(p[i][y_count][0]);            //
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
            
//            _normals->push_back(getNormal(*tverts, p[ii][j][0], p[ii][j][1], p[ii][j+1][1]));
            _normals->push_back(getNormal(*tverts, p[ii][j][0], p[ii][j+1][1], p[ii][j][1]));
            _nbinds->push_back(_normals->size()-1);
            _nbinds->push_back(_normals->size()-1);
            _nbinds->push_back(_normals->size()-1);
            _nbinds->push_back(_normals->size()-1);
            
//            _lines->push_back(p[ii][j][0]);
//            _lines->push_back(p[ii][j][1]);             //
//            _lines->push_back(p[ii][j][1]);
//            _lines->push_back(p[ii][j+1][1]);           //
//            _lines->push_back(p[ii][j+1][1]);
//            _lines->push_back(p[ii][j+1][0]);           //
//            _lines->push_back(p[ii][j+1][0]);
//            _lines->push_back(p[ii][j][0]);             //
        }
    }

    _geom->setVertexArray(_verts);
    _geom->addPrimitiveSet(_faces);
    
    _geom->setNormalArray(_normals);
    _geom->setNormalIndices(_nbinds);
    _geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
    
    _geom->setColorArray(_colors);
    _geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    
    _lineGeom->setVertexArray(_verts);
    _lineGeom->addPrimitiveSet(_lines);
    _lineGeom->setColorArray(_colors);
    _lineGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    

    addDrawable(_geom);
    addDrawable(_lineGeom);
}













