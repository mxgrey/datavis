
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

    assemble();
}

Surf3D::~Surf3D()
{
//    delete _geom;
//    delete _verts;
//    delete _colors;
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
        if(values.size() != layersize)
        {
            std::cerr << "Size of layer " << i << " (" << values.size()
                      << ") does not match the rest (" << layersize <<  ")!"
                      << std::endl;
        }
        double z = values[0];
        double x=0, y=0;
        for(size_t j=0; j<pointcount; ++j)
        {
            x = values[2*j+1];
            y = values[2*j+2];

            _verts->push_back(osg::Vec3(x,y,z));
        }
    }

    for(size_t i=0; i<data.size()-1; ++i)
    {
        for(size_t j=0; j<pointcount-1; ++j)
        {
            _faces->push_back(layersize*i + j);
            _faces->push_back(layersize*i + j+1);
            _faces->push_back(layersize*(i+1) + j);
            _faces->push_back(layersize*(i+1) + j+1);
        }
    }

}













