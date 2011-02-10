// $Id$
// 
// Test Suite for geos::operation::geounion::CascadedPolygonUnion class.

// tut
#include <tut.hpp>
// geos
#include <geos/operation/union/CascadedPolygonUnion.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/Point.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
// std
#include <memory>
#include <string>
#include <vector>

namespace tut
{
    //
    // Test Group
    //

    // Common data used by tests
    struct test_cascadedpolygonuniontest_data
    {
        geos::geom::GeometryFactory gf;
        geos::io::WKTReader wktreader;
        geos::io::WKTWriter wktwriter;

        typedef geos::geom::Geometry::AutoPtr GeomPtr;

        test_cascadedpolygonuniontest_data()
          : gf(),
            wktreader(&gf)
        {}
    };

    typedef test_group<test_cascadedpolygonuniontest_data> group;
    typedef group::object object;

    group test_cascadedpolygonuniontest_group("geos::operation::geounion::CascadedPolygonUnion");

    // test runner
    geos::geom::Geometry* unionIterated(
        std::vector<geos::geom::Polygon*>* geoms)
    {
        typedef std::vector<geos::geom::Polygon*>::iterator iterator;

        std::auto_ptr<geos::geom::Geometry> unionAll;
        iterator end = geoms->end();
        for (iterator i = geoms->begin(); i != end; ++i)
        {
            if (!unionAll.get())
            {
                unionAll.reset((*i)->clone());
            }
            else
            {
                unionAll.reset(unionAll->Union(*i));
            }
        }
        return unionAll.release();
    }

    geos::geom::Geometry* unionCascaded(
        std::vector<geos::geom::Polygon*>* geoms)
    {
        using geos::operation::geounion::CascadedPolygonUnion;
        return CascadedPolygonUnion::Union(geoms);
    }

    void test_runner(test_cascadedpolygonuniontest_data& t,
        std::vector<geos::geom::Polygon*>* geoms) 
    {
        std::auto_ptr<geos::geom::Geometry> union1(unionIterated(geoms));
        std::auto_ptr<geos::geom::Geometry> union2(unionCascaded(geoms));

        // For now we compare the WKT representations of the two generated 
        // geometries which works well for simple geometries only.
        // More complex geometries require to use special similarity measure
        // criteria instead.
        std::string iterated(t.wktwriter.writeFormatted(union1.get()));
        std::string cascaded(t.wktwriter.writeFormatted(union2.get()));

        ensure_equals(iterated, cascaded);
    }

    void delete_geometry(geos::geom::Geometry* g)
    {
        delete g;
    }

    //
    // Test Cases
    //

    template<>
    template<>
    void object::test<1>()
    {
        static char const* const polygons[] = 
        {
            "POLYGON ((80 260, 200 260, 200 30, 80 30, 80 260))",
            "POLYGON ((30 180, 300 180, 300 110, 30 110, 30 180))",
            "POLYGON ((30 280, 30 150, 140 150, 140 280, 30 280))",
            NULL
        };

        std::vector<geos::geom::Polygon*> g;
        for (char const* const* p = polygons; *p != NULL; ++p)
        {
            std::string wkt(*p);
            geos::geom::Polygon* geom = 
                dynamic_cast<geos::geom::Polygon*>(wktreader.read(wkt));
            g.push_back(geom);
        }

        test_runner(*this, &g); 

        for_each(g.begin(), g.end(), delete_geometry);
    }

    void create_discs(geos::geom::GeometryFactory& gf, int num, double radius, 
        std::vector<geos::geom::Polygon*>* g)
    {
        for (int i = 0; i < num; ++i) {
            for (int j = 0; j < num; ++j) {
                std::auto_ptr<geos::geom::Point> pt(
                    gf.createPoint(geos::geom::Coordinate(i, j)));
                g->push_back(dynamic_cast<geos::geom::Polygon*>(pt->buffer(radius)));
            }
        }
    }

// these tests currently fail because the geometries generated by the different 
// union algoritms are slightly different. In order to make those tests pass 
// we need to port the similarity measure classes from JTS, allowing to 
// approximately compare the two results 

//     template<>
//     template<>
//     void object::test<2>()
//     {
//         std::vector<geos::geom::Polygon*> g;
//         create_discs(gf, 5, 0.7, &g);
// 
//         test_runner(*this, &g); 
// 
//         std::for_each(g.begin(), g.end(), delete_geometry);
//     }

//     template<>
//     template<>
//     void object::test<3>()
//     {
//         std::vector<geos::geom::Polygon*> g;
//         create_discs(gf, 5, 0.55, &g);
// 
//         test_runner(*this, &g); 
// 
//         std::for_each(g.begin(), g.end(), delete_geometry);
//     }

} // namespace tut

