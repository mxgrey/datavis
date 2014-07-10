#ifndef SURF3D_HPP
#define SURF3D_HPP

#include <osg/Geode>

#include <string>
#include <vector>

namespace datavis {

class Surf3D : public osg::Geode
{
public:

    Surf3D();
    Surf3D(const std::string& filename, osg::Vec4 color = osg::Vec4(0,0,1,1));

    std::vector< std::vector<double> > data;
    osg::Vec4 color;

    void assemble();


protected:
    
    virtual ~Surf3D();

    osg::ref_ptr<osg::Geometry> _geom;
    osg::ref_ptr<osg::Vec3Array> _verts;
    osg::ref_ptr<osg::Vec4Array> _colors;
    osg::ref_ptr<osg::DrawElementsUInt> _faces;

};


} // namespace datavis

#endif // SURF3D_HPP
