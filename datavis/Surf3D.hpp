#ifndef SURF3D_HPP
#define SURF3D_HPP

#include <osg/Geode>

#include <string>
#include <vector>

namespace datavis {

class Surf3D : osg::Geode
{
public:

    Surf3D();
    Surf3D(const std::string& filename, osg::Vec4 color = osg::Vec4(0,0,1,1));
    virtual ~Surf3D();

    std::vector< std::vector<double> > data;
    osg::Vec4 color;

    void assemble();


protected:

    osg::Geometry* _geom;
    osg::Vec3Array* _verts;
    osg::Vec4Array* _colors;
    osg::DrawElementsUInt* _faces;

};


} // namespace datavis

#endif // SURF3D_HPP
