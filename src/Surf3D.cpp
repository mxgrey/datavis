
#include "datavis/Surf3D.hpp"

#include <osg/Geometry>


#include <fstream>
#include <sstream>
#include <iostream>


using namespace datavis;

Surf3D::Surf3D()
{

}

Surf3D::Surf3D(const std::string &filename, osg::Vec4 color) :
    color(color)
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
    while(filestream.good())
    {
        dataline.clear();
        std::getline(filestream, fileline);

        std::stringstream datastream(fileline);

        while(datastream.good())
        {
            datastream >> value;
            dataline.push_back(value);
        }

        data.push_back(dataline);
    }
    
    filestream.close();

    assemble();
}

Surf3D::~Surf3D()
{
    
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

    _geom = new osg::Geometry;
    _verts = new osg::Vec3Array;
    _colors = new osg::Vec4Array;
    _colors->push_back(color);

    _faces = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);


    for(size_t i=0; i<data.size(); ++i)
    {
        const std::vector<double>& values = data[i];
        std::vector< std::vector<unsigned int> > row;
        if(values.size() != layersize)
        {
            std::cerr << "Size of layer " << i << " (" << values.size()
                      << ") does not match the rest (" << layersize <<  ")!"
                      << std::endl;
        }
        double z = values[0];
        double x=0, y=0;

        std::vector<unsigned int> points;
        if(pointcount%2 != 0)
            std::cout << "Expected an even number of points per row, but got "
                      << pointcount << " for row #" << i << std::endl;

        for(size_t j=0; j<pointcount; ++j)
        {
            x = values[2*j+1];
            y = values[2*j+2];

            _verts->push_back(osg::Vec3(x,y,z));

            if(j%2==0)
            {
                points.clear();
                points.push_back(_verts->size()-1);
            }
            else
            {
                points.push_back(_verts->size()-1);
                row.push_back(points);
                points.clear();
            }
        }

        p.push_back(row);
    }

    for(size_t i=0; i<data.size()-1; ++i)
    {
        for(size_t j=0; j<(pointcount-1)/2; ++j)
        {
            for(size_t k=0; k<2; ++k)
            {
                _faces->push_back(p[i][j][k]);
                _faces->push_back(p[i][j+1][k]);
                _faces->push_back(p[i+1][j+1][k]);
                _faces->push_back(p[i+1][j][k]);
            }
        }
    }

    _geom->setVertexArray(_verts);
    _geom->addPrimitiveSet(_faces);

    _geom->setColorArray(_colors);
    _geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

    this->addDrawable(_geom);
}













