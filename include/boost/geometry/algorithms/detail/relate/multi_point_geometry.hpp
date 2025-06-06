// Boost.Geometry

// Copyright (c) 2014-2023, Oracle and/or its affiliates.

// Contributed and/or modified by Vissarion Fysikopoulos, on behalf of Oracle
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_DETAIL_RELATE_MULTI_POINT_GEOMETRY_HPP
#define BOOST_GEOMETRY_ALGORITHMS_DETAIL_RELATE_MULTI_POINT_GEOMETRY_HPP


#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/size.hpp>
#include <boost/range/value_type.hpp>

#include <boost/geometry/algorithms/detail/disjoint/box_box.hpp>
#include <boost/geometry/algorithms/detail/disjoint/point_box.hpp>
#include <boost/geometry/algorithms/detail/expand_by_epsilon.hpp>
#include <boost/geometry/algorithms/detail/partition.hpp>
#include <boost/geometry/algorithms/detail/relate/result.hpp>
#include <boost/geometry/algorithms/detail/relate/topology_check.hpp>
#include <boost/geometry/algorithms/detail/within/point_in_geometry.hpp>
#include <boost/geometry/algorithms/envelope.hpp>

#include <boost/geometry/core/point_type.hpp>

#include <boost/geometry/geometries/box.hpp>

#include <boost/geometry/index/rtree.hpp>

// TEMP
#include <boost/geometry/strategies/envelope/cartesian.hpp>
#include <boost/geometry/strategies/envelope/geographic.hpp>
#include <boost/geometry/strategies/envelope/spherical.hpp>

#include <boost/geometry/util/type_traits.hpp>


namespace boost { namespace geometry
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace relate
{

template
<
    typename Geometry,
    typename Tag = tag_t<Geometry>
>
struct multi_point_geometry_eb
{
    template <typename MultiPoint, typename Strategy>
    static inline bool apply(MultiPoint const& ,
                             detail::relate::topology_check<Geometry, Strategy> const& )
    {
        return true;
    }
};

template <typename Geometry>
struct multi_point_geometry_eb<Geometry, linestring_tag>
{
    template <typename Points>
    struct boundary_visitor
    {
        boundary_visitor(Points const& points)
            : m_points(points)
            , m_boundary_found(false)
        {}

        template <typename Point, typename Strategy>
        struct find_pred
        {
            find_pred(Point const& point, Strategy const& strategy)
                : m_point(point)
                , m_strategy(strategy)
            {}

            template <typename Pt>
            bool operator()(Pt const& pt) const
            {
                return detail::equals::equals_point_point(pt, m_point, m_strategy);
            }

            Point const& m_point;
            Strategy const& m_strategy;
        };

        template <typename Point, typename Strategy>
        bool apply(Point const& boundary_point, Strategy const& strategy)
        {
            if ( std::none_of(m_points.begin(), m_points.end(),
                              find_pred<Point, Strategy>(boundary_point, strategy)))
            {
                m_boundary_found = true;
                return false;
            }
            return true;
        }

        bool result() const { return m_boundary_found; }

    private:
        Points const& m_points;
        bool m_boundary_found;
    };

    template <typename MultiPoint, typename Strategy>
    static inline bool apply(MultiPoint const& multi_point,
                             detail::relate::topology_check<Geometry, Strategy> const& tc)
    {
        boundary_visitor<MultiPoint> visitor(multi_point);
        tc.for_each_boundary_point(visitor);
        return visitor.result();
    }
};

template <typename Geometry>
struct multi_point_geometry_eb<Geometry, multi_linestring_tag>
{
    template <typename Points>
    struct boundary_visitor
    {
        boundary_visitor(Points const& points)
            : m_points(points)
            , m_boundary_found(false)
        {}

        template <typename Point, typename Strategy>
        bool apply(Point const& boundary_point, Strategy const&)
        {
            typedef geometry::less<void, -1, Strategy> less_type;

            if (! std::binary_search(m_points.begin(), m_points.end(),
                                     boundary_point, less_type()) )
            {
                m_boundary_found = true;
                return false;
            }
            return true;
        }

        bool result() const { return m_boundary_found; }

    private:
        Points const& m_points;
        bool m_boundary_found;
    };

    template <typename MultiPoint, typename Strategy>
    static inline bool apply(MultiPoint const& multi_point,
                             detail::relate::topology_check<Geometry, Strategy> const& tc)
    {
        typedef typename boost::range_value<MultiPoint>::type point_type;
        typedef std::vector<point_type> points_type;
        typedef geometry::less<void, -1, Strategy> less_type;

        points_type points(boost::begin(multi_point), boost::end(multi_point));
        std::sort(points.begin(), points.end(), less_type());

        boundary_visitor<points_type> visitor(points);
        tc.for_each_boundary_point(visitor);
        return visitor.result();
    }
};

// SingleGeometry - Linear or Areal
template <typename MultiPoint, typename SingleGeometry, bool Transpose = false>
struct multi_point_single_geometry
{
    static const bool interruption_enabled = true;

    template <typename Result, typename Strategy>
    static inline void apply(MultiPoint const& multi_point,
                             SingleGeometry const& single_geometry,
                             Result & result,
                             Strategy const& strategy)
    {
        using box2_type = model::box<point_type_t<SingleGeometry>> ;

        box2_type box2;
        geometry::envelope(single_geometry, box2, strategy);
        geometry::detail::expand_by_epsilon(box2);

        for (auto it = boost::begin(multi_point); it != boost::end(multi_point); ++it)
        {
            if (! (relate::may_update<interior, interior, '0', Transpose>(result)
                || relate::may_update<interior, boundary, '0', Transpose>(result)
                || relate::may_update<interior, exterior, '0', Transpose>(result) ) )
            {
                break;
            }

            // The default strategy is enough for Point/Box
            if (detail::disjoint::disjoint_point_box(*it, box2, strategy))
            {
                update<interior, exterior, '0', Transpose>(result);
            }
            else
            {
                int in_val = detail::within::point_in_geometry(*it, single_geometry, strategy);

                if (in_val > 0) // within
                {
                    update<interior, interior, '0', Transpose>(result);
                }
                else if (in_val == 0)
                {
                    update<interior, boundary, '0', Transpose>(result);
                }
                else // in_val < 0 - not within
                {
                    update<interior, exterior, '0', Transpose>(result);
                }
            }

            if ( BOOST_GEOMETRY_CONDITION(result.interrupt) )
            {
                return;
            }
        }

        typedef detail::relate::topology_check<SingleGeometry, Strategy> tc_t;

        if ( relate::may_update<exterior, interior, tc_t::interior, Transpose>(result)
          || relate::may_update<exterior, boundary, tc_t::boundary, Transpose>(result) )
        {
            tc_t tc(single_geometry, strategy);

            if ( relate::may_update<exterior, interior, tc_t::interior, Transpose>(result)
              && tc.has_interior() )
            {
                // TODO: this is not true if a linestring is degenerated to a point
                // then the interior has topological dimension = 0, not 1
                update<exterior, interior, tc_t::interior, Transpose>(result);
            }

            if ( relate::may_update<exterior, boundary, tc_t::boundary, Transpose>(result)
              && tc.has_boundary() )
            {
                if (multi_point_geometry_eb<SingleGeometry>::apply(multi_point, tc))
                {
                    update<exterior, boundary, tc_t::boundary, Transpose>(result);
                }
            }
        }

        update<exterior, exterior, result_dimension<MultiPoint>::value, Transpose>(result);
    }
};


// MultiGeometry - Linear or Areal
// part of the algorithm calculating II and IB when no IE has to be calculated
// using partition()
template <typename MultiPoint, typename MultiGeometry, bool Transpose>
class multi_point_multi_geometry_ii_ib
{
    template <typename Strategy>
    struct expand_box_point
    {
        expand_box_point(Strategy const& strategy)
            : m_strategy(strategy)
        {}

        template <typename Box, typename Point>
        inline void apply(Box& total, Point const& point) const
        {
            geometry::expand(total, point, m_strategy);
        }

    private:
        Strategy const& m_strategy;
    };

    template <typename Strategy>
    struct expand_box_box_pair
    {
        expand_box_box_pair(Strategy const& strategy)
            : m_strategy(strategy)
        {}

        template <typename Box, typename BoxPair>
        inline void apply(Box& total, BoxPair const& box_pair) const
        {
            geometry::expand(total, box_pair.first, m_strategy);
        }

    private:
        Strategy const& m_strategy;
    };

    template <typename Strategy>
    struct overlaps_box_point
    {
        overlaps_box_point(Strategy const& strategy)
            : m_strategy(strategy)
        {}

        template <typename Box, typename Point>
        inline bool apply(Box const& box, Point const& point) const
        {
            // The default strategy is enough for Point/Box
            return ! detail::disjoint::disjoint_point_box(point, box,
                                                          m_strategy);
        }

    private:
        Strategy const& m_strategy;
    };

    template <typename Strategy>
    struct overlaps_box_box_pair
    {
        overlaps_box_box_pair(Strategy const& strategy)
            : m_strategy(strategy)
        {}

        template <typename Box, typename BoxPair>
        inline bool apply(Box const& box, BoxPair const& box_pair) const
        {
            // The default strategy is enough for Box/Box
            return ! detail::disjoint::disjoint_box_box(box_pair.first, box,
                                                        m_strategy);
        }

    private:
        Strategy const& m_strategy;
    };

    template <typename Result, typename Strategy>
    class item_visitor_type
    {
        typedef detail::relate::topology_check<MultiGeometry, Strategy> topology_check_type;

    public:
        item_visitor_type(MultiGeometry const& multi_geometry,
                          topology_check_type const& tc,
                          Result & result,
                          Strategy const& strategy)
            : m_multi_geometry(multi_geometry)
            , m_tc(tc)
            , m_result(result)
            , m_strategy(strategy)
        {}

        template <typename Point, typename BoxPair>
        inline bool apply(Point const& point, BoxPair const& box_pair)
        {
            // The default strategy is enough for Point/Box
            if (! detail::disjoint::disjoint_point_box(point, box_pair.first, m_strategy) )
            {
                typename boost::range_value<MultiGeometry>::type const&
                    single = range::at(m_multi_geometry, box_pair.second);

                int in_val = detail::within::point_in_geometry(point, single, m_strategy);

                if (in_val > 0) // within
                {
                    update<interior, interior, '0', Transpose>(m_result);
                }
                else if (in_val == 0)
                {
                    if (m_tc.check_boundary_point(point))
                    {
                        update<interior, boundary, '0', Transpose>(m_result);
                    }
                    else
                    {
                        update<interior, interior, '0', Transpose>(m_result);
                    }
                }
            }

            if ( BOOST_GEOMETRY_CONDITION(m_result.interrupt) )
            {
                return false;
            }

            if (! (relate::may_update<interior, interior, '0', Transpose>(m_result)
                || relate::may_update<interior, boundary, '0', Transpose>(m_result) ) )
            {
                return false;
            }

            return true;
        }


    private:
        MultiGeometry const& m_multi_geometry;
        topology_check_type const& m_tc;
        Result & m_result;
        Strategy const& m_strategy;
    };

public:
    using box1_type = model::box<point_type_t<MultiPoint>>;
    using box2_type = model::box<point_type_t<MultiGeometry>>;
    using box_pair_type = std::pair<box2_type, std::size_t>;

    template <typename Result, typename Strategy>
    static inline void apply(MultiPoint const& multi_point,
                             MultiGeometry const& multi_geometry,
                             std::vector<box_pair_type> const& boxes,
                             detail::relate::topology_check
                                <
                                    MultiGeometry, Strategy
                                > const& tc,
                             Result & result,
                             Strategy const& strategy)
    {
        item_visitor_type<Result, Strategy> visitor(multi_geometry, tc, result, strategy);

        geometry::partition
            <
                box1_type
            >::apply(multi_point, boxes, visitor,
                     expand_box_point<Strategy>(strategy),
                     overlaps_box_point<Strategy>(strategy),
                     expand_box_box_pair<Strategy>(strategy),
                     overlaps_box_box_pair<Strategy>(strategy));
    }

};

// MultiGeometry - Linear or Areal
// part of the algorithm calculating II, IB and IE
// using rtree
template <typename MultiPoint, typename MultiGeometry, bool Transpose>
struct multi_point_multi_geometry_ii_ib_ie
{
    using box1_type = model::box<point_type_t<MultiPoint>>;
    using box2_type = model::box<point_type_t<MultiGeometry>>;
    using box_pair_type = std::pair<box2_type, std::size_t>;
    using boxes_type = std::vector<box_pair_type>;

    template <typename Result, typename Strategy>
    static inline void apply(MultiPoint const& multi_point,
                             MultiGeometry const& multi_geometry,
                             std::vector<box_pair_type> const& boxes,
                             detail::relate::topology_check
                                <
                                    MultiGeometry, Strategy
                                > const& tc,
                             Result & result,
                             Strategy const& strategy)
    {
        typedef index::parameters
            <
                index::rstar<4>, Strategy
            > index_parameters_type;
        index::rtree<box_pair_type, index_parameters_type>
            rtree(boxes.begin(), boxes.end(),
                  index_parameters_type(index::rstar<4>(), strategy));

        for (auto it = boost::begin(multi_point); it != boost::end(multi_point); ++it)
        {
            if (! (relate::may_update<interior, interior, '0', Transpose>(result)
                || relate::may_update<interior, boundary, '0', Transpose>(result)
                || relate::may_update<interior, exterior, '0', Transpose>(result) ) )
            {
                return;
            }

            typename boost::range_value<MultiPoint>::type const& point = *it;

            boxes_type boxes_found;
            rtree.query(index::intersects(point), std::back_inserter(boxes_found));

            bool found_ii_or_ib = false;
            for (auto const& box_found : boxes_found)
            {
                typename boost::range_value<MultiGeometry>::type const&
                    single = range::at(multi_geometry, box_found.second);

                int in_val = detail::within::point_in_geometry(point, single, strategy);

                if (in_val > 0) // within
                {
                    update<interior, interior, '0', Transpose>(result);
                    found_ii_or_ib = true;
                }
                else if (in_val == 0) // on boundary of single
                {
                    if (tc.check_boundary_point(point))
                    {
                        update<interior, boundary, '0', Transpose>(result);
                    }
                    else
                    {
                        update<interior, interior, '0', Transpose>(result);
                    }
                    found_ii_or_ib = true;
                }
            }

            // neither interior nor boundary found -> exterior
            if (found_ii_or_ib == false)
            {
                update<interior, exterior, '0', Transpose>(result);
            }

            if ( BOOST_GEOMETRY_CONDITION(result.interrupt) )
            {
                return;
            }
        }
    }
};

// MultiGeometry - Linear or Areal
template <typename MultiPoint, typename MultiGeometry, bool Transpose = false>
struct multi_point_multi_geometry
{
    static const bool interruption_enabled = true;

    template <typename Result, typename Strategy>
    static inline void apply(MultiPoint const& multi_point,
                             MultiGeometry const& multi_geometry,
                             Result & result,
                             Strategy const& strategy)
    {
        using box_pair_type = std::pair<model::box<point_type_t<MultiGeometry>>, std::size_t>;

        std::size_t count2 = boost::size(multi_geometry);
        std::vector<box_pair_type> boxes(count2);
        for (std::size_t i = 0 ; i < count2 ; ++i)
        {
            geometry::envelope(range::at(multi_geometry, i), boxes[i].first, strategy);
            geometry::detail::expand_by_epsilon(boxes[i].first);
            boxes[i].second = i;
        }

        typedef detail::relate::topology_check<MultiGeometry, Strategy> tc_t;
        tc_t tc(multi_geometry, strategy);

        if ( relate::may_update<interior, interior, '0', Transpose>(result)
          || relate::may_update<interior, boundary, '0', Transpose>(result)
          || relate::may_update<interior, exterior, '0', Transpose>(result) )
        {
            // If there is no need to calculate IE, use partition
            if (! relate::may_update<interior, exterior, '0', Transpose>(result) )
            {
                multi_point_multi_geometry_ii_ib<MultiPoint, MultiGeometry, Transpose>
                    ::apply(multi_point, multi_geometry, boxes, tc, result, strategy);
            }
            else // otherwise use rtree
            {
                multi_point_multi_geometry_ii_ib_ie<MultiPoint, MultiGeometry, Transpose>
                    ::apply(multi_point, multi_geometry, boxes, tc, result, strategy);
            }
        }

        if ( BOOST_GEOMETRY_CONDITION(result.interrupt) )
        {
            return;
        }

        if ( relate::may_update<exterior, interior, tc_t::interior, Transpose>(result)
          || relate::may_update<exterior, boundary, tc_t::boundary, Transpose>(result) )
        {
            if ( relate::may_update<exterior, interior, tc_t::interior, Transpose>(result)
              && tc.has_interior() )
            {
                // TODO: this is not true if a linestring is degenerated to a point
                // then the interior has topological dimension = 0, not 1
                update<exterior, interior, tc_t::interior, Transpose>(result);
            }

            if ( relate::may_update<exterior, boundary, tc_t::boundary, Transpose>(result)
              && tc.has_boundary() )
            {
                if (multi_point_geometry_eb<MultiGeometry>::apply(multi_point, tc))
                {
                    update<exterior, boundary, tc_t::boundary, Transpose>(result);
                }
            }
        }

        update<exterior, exterior, result_dimension<MultiPoint>::value, Transpose>(result);
    }

};


template
<
    typename MultiPoint, typename Geometry,
    bool Transpose = false,
    bool isMulti = util::is_multi<Geometry>::value
>
struct multi_point_geometry
    : multi_point_single_geometry<MultiPoint, Geometry, Transpose>
{};

template <typename MultiPoint, typename Geometry, bool Transpose>
struct multi_point_geometry<MultiPoint, Geometry, Transpose, true>
    : multi_point_multi_geometry<MultiPoint, Geometry, Transpose>
{};


// transposed result of multi_point_geometry
template <typename Geometry, typename MultiPoint>
struct geometry_multi_point
{
    static const bool interruption_enabled = true;

    template <typename Result, typename Strategy>
    static inline void apply(Geometry const& geometry, MultiPoint const& multi_point,
                             Result & result, Strategy const& strategy)
    {
        multi_point_geometry<MultiPoint, Geometry, true>::apply(multi_point, geometry, result, strategy);
    }
};

}} // namespace detail::relate
#endif // DOXYGEN_NO_DETAIL

}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_ALGORITHMS_DETAIL_RELATE_MULTI_POINT_GEOMETRY_HPP
