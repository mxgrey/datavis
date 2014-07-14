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
    Surf3D(const std::string& filename, osg::Vec4 startColor = osg::Vec4(0,0,1,1),
                                        osg::Vec4 finalColor = osg::Vec4(1,0,0,1));
    
    double x_scale;
    double y_scale;
    double z_scale;

    std::vector< std::vector<double> > data;

    std::vector< std::vector< std::vector<unsigned int> > > p;

    osg::Vec4 startColor;
    osg::Vec4 finalColor;
    osg::Vec4 getColor(size_t layer);

    void assemble();


protected:
    
    virtual ~Surf3D();

    osg::ref_ptr<osg::Geometry> _geom;
    osg::ref_ptr<osg::Geometry> _lineGeom;
    osg::ref_ptr<osg::Vec3Array> _verts;
    osg::ref_ptr<osg::Vec3Array> _normals;
    osg::ref_ptr<osg::Vec4Array> _colors;
    osg::ref_ptr<osg::Vec4Array> _lineColors;
    osg::ref_ptr<osg::DrawElementsUInt> _faces;
    osg::ref_ptr<osg::DrawElementsUInt> _lines;
    osg::ref_ptr<osg::UIntArray> _nbinds;

};


} // namespace datavis

#endif // SURF3D_HPP
