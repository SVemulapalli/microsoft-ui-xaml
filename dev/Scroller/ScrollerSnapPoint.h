// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "SnapPointBase.g.h"
#include "ScrollSnapPointBase.g.h"
#include "ScrollSnapPoint.g.h"
#include "RepeatedScrollSnapPoint.g.h"
#include "ZoomSnapPointBase.g.h"
#include "ZoomSnapPoint.g.h"
#include "RepeatedZoomSnapPoint.g.h"

enum class ScrollerSnapPointApplicableRangeType
{
    Mandatory = 0,
    Optional = 1,
};

struct ScrollerSnapPointSortPredicate
{
    //Sorting of snap points goes as follows:
    //We first sort on the primary value which for regular snap points are their actualStart values, and for irregular snap points their values.
    //We then sort by the secondary value which for regular snap points are their actualEnd values, and for irregular snap points are again their values.
    //We then sort by the tertiary value which for regular snap points is 1 and for irregular snap points is 0.

    //Since regular snap points' actualEnd is always greater than or equal to their actualStart the tertiary value is only used if these values are exactly equal
    //for a regular snap point which also shares a primary (and subsequently secondary) value with an irregular snap point's value.

    //The result of this sorting is irregular snap points will always be sorted before regular snap points when ambiguity occurs.
    //This allows us to address some corner cases in a more elegant fashion.
    double primary;
    double secondary;
    short tertiary;
};

class SnapPointBase :
    public ReferenceTracker<SnapPointBase, winrt::implementation::SnapPointBaseT, winrt::composable>
{
public:
    double ApplicableRange();
    ScrollerSnapPointApplicableRangeType ApplicableRangeType();

    //Internal
    bool operator< (SnapPointBase* snapPoint);
    bool operator== (SnapPointBase* snapPoint);

    virtual winrt::ExpressionAnimation CreateRestingPointExpression(winrt::Compositor compositor, winrt::hstring target, winrt::hstring scale) = 0;
    virtual winrt::ExpressionAnimation CreateConditionalExpression(winrt::Compositor compositor, winrt::hstring target, winrt::hstring scale) = 0;
    virtual ScrollerSnapPointSortPredicate SortPredicate() = 0;
    virtual void DetermineActualApplicableZone(SnapPointBase* previousSnapPoint, SnapPointBase* nextSnapPoint) = 0;
    virtual double Influence(double edgeOfMidpoint) = 0;
    virtual void Combine(winrt::SnapPointBase const& snapPoint) = 0;
    virtual double Evaluate(double value) = 0;
    int CombinationCount();
#ifdef _DEBUG
    winrt::Color VisualizationColor();
    void VisualizationColor(winrt::Color color);
#endif // _DEBUG
    std::tuple<double, double> ActualApplicableZone();
    void ActualApplicableZone(std::tuple<double, double> range);

protected:
    // Needed as work around for Modern Idl inheritance bug
    SnapPointBase();

    winrt::hstring GetTargetExpression(winrt::hstring target) const;

    double m_specifiedApplicableRange{ INFINITY };
    std::tuple<double, double> m_actualApplicableZone{ -INFINITY, INFINITY };
    ScrollerSnapPointApplicableRangeType m_applicableRangeType{ ScrollerSnapPointApplicableRangeType::Mandatory };
    int m_combinationCount{ 0 };
#ifdef _DEBUG
    winrt::Color m_visualizationColor{ winrt::Colors::Black() };
#endif // _DEBUG

    // Maximum difference for snap points to be considered equal
    static constexpr double s_equalityEpsilon{ 0.00001 };
};

class ScrollSnapPointBase :
    public winrt::implementation::ScrollSnapPointBaseT<ScrollSnapPointBase, SnapPointBase>
{
public:
    winrt::SnapPointsAlignment Alignment();
    void Alignment(winrt::SnapPointsAlignment alignment);

protected:
    // Needed as work around for Modern Idl inheritance bug
    ScrollSnapPointBase();

    winrt::SnapPointsAlignment m_alignment{ winrt::SnapPointsAlignment::Near };
};

class ScrollSnapPoint:
    public winrt::implementation::ScrollSnapPointT<ScrollSnapPoint, ScrollSnapPointBase>
{
public:
    ScrollSnapPoint(
        double snapPointValue,
        winrt::SnapPointsAlignment alignment = winrt::SnapPointsAlignment::Near);

    ScrollSnapPoint(
        double snapPointValue,
        double applicableRange,
        winrt::SnapPointsAlignment alignment = winrt::SnapPointsAlignment::Near);

    double Value();

    //Internal
    winrt::ExpressionAnimation CreateRestingPointExpression(winrt::Compositor compositor, winrt::hstring, winrt::hstring scale);
    winrt::ExpressionAnimation CreateConditionalExpression(winrt::Compositor compositor, winrt::hstring target, winrt::hstring scale);
    ScrollerSnapPointSortPredicate SortPredicate();
    void DetermineActualApplicableZone(SnapPointBase* previousSnapPoint, SnapPointBase* nextSnapPoint);
    double Influence(double edgeOfMidpoint);
    void Combine(winrt::SnapPointBase const& snapPoint);
    double Evaluate(double value);
private:
    double DetermineMinActualApplicableZone(SnapPointBase* previousSnapPoint);
    double DetermineMaxActualApplicableZone(SnapPointBase* nextSnapPoint);

    double m_specifiedValue{ 0.0 };
    double m_actualValue{ 0.0 };
};

class RepeatedScrollSnapPoint :
    public winrt::implementation::RepeatedScrollSnapPointT<RepeatedScrollSnapPoint, ScrollSnapPointBase>
{
public:
    RepeatedScrollSnapPoint(
        double offset,
        double interval,
        double start,
        double end,
        winrt::SnapPointsAlignment alignment = winrt::SnapPointsAlignment::Near);

    RepeatedScrollSnapPoint(
        double offset,
        double interval,
        double start,
        double end,
        double applicableRange,
        winrt::SnapPointsAlignment alignment = winrt::SnapPointsAlignment::Near);

    double Offset();
    double Interval();
    double Start();
    double End();

    //Internal
    winrt::ExpressionAnimation CreateRestingPointExpression(winrt::Compositor compositor, winrt::hstring target, winrt::hstring scale);
    winrt::ExpressionAnimation CreateConditionalExpression(winrt::Compositor compositor, winrt::hstring target, winrt::hstring scale);
    ScrollerSnapPointSortPredicate SortPredicate();
    void DetermineActualApplicableZone(SnapPointBase* previousSnapPoint, SnapPointBase* nextSnapPoint);
    double Influence(double edgeOfMidpoint);
    void Combine(winrt::SnapPointBase const& snapPoint);
    double Evaluate(double value);

    double ActualStart();
    void ActualStart(double start);
    double ActualEnd();
    void ActualEnd(double end);

private:
    double DetermineFirstRepeatedSnapPointValue();
    double DetermineMinActualApplicableZone(SnapPointBase* previousSnapPoint);
    double DetermineMaxActualApplicableZone(SnapPointBase* nextSnapPoint);
    void ValidateConstructorParameters(
        double offset,
        double interval,
        double start,
        double end,
        double applicableRange,
        bool applicableRangeTypeToo);

    double m_offset{ 0.0f };
    double m_interval{ 0.0f };
    double m_specifiedStart{ 0.0f };
    double m_actualStart{ 0.0f };
    double m_specifiedEnd{ 0.0f };
    double m_actualEnd{ 0.0f };
};

class ZoomSnapPointBase :
    public winrt::implementation::ZoomSnapPointBaseT<ZoomSnapPointBase, SnapPointBase>
{
protected:
    // Needed as work around for Modern Idl inheritance bug
    ZoomSnapPointBase();
};

class ZoomSnapPoint :
    public winrt::implementation::ZoomSnapPointT<ZoomSnapPoint, ZoomSnapPointBase>
{
public:
    ZoomSnapPoint(
        double snapPointValue);

    ZoomSnapPoint(
        double snapPointValue,
        double applicableRange);

    double Value();

    //Internal
    winrt::ExpressionAnimation CreateRestingPointExpression(winrt::Compositor compositor, winrt::hstring, winrt::hstring scale);
    winrt::ExpressionAnimation CreateConditionalExpression(winrt::Compositor compositor, winrt::hstring target, winrt::hstring scale);
    ScrollerSnapPointSortPredicate SortPredicate();
    void DetermineActualApplicableZone(SnapPointBase* previousSnapPoint, SnapPointBase* nextSnapPoint);
    double Influence(double edgeOfMidpoint);
    void Combine(winrt::SnapPointBase const& snapPoint);
    double Evaluate(double value);
private:
    double DetermineMinActualApplicableZone(SnapPointBase* previousSnapPoint);
    double DetermineMaxActualApplicableZone(SnapPointBase* nextSnapPoint);

    double m_specifiedValue{ 0.0 };
    double m_actualValue{ 0.0 };
};

class RepeatedZoomSnapPoint :
    public winrt::implementation::RepeatedZoomSnapPointT<RepeatedZoomSnapPoint, ZoomSnapPointBase>
{
public:
    RepeatedZoomSnapPoint(
        double offset,
        double interval,
        double start,
        double end);

    RepeatedZoomSnapPoint(
        double offset,
        double interval,
        double start,
        double end,
        double applicableRange);

    double Offset();
    double Interval();
    double Start();
    double End();

    //Internal
    winrt::ExpressionAnimation CreateRestingPointExpression(winrt::Compositor compositor, winrt::hstring target, winrt::hstring scale);
    winrt::ExpressionAnimation CreateConditionalExpression(winrt::Compositor compositor, winrt::hstring target, winrt::hstring scale);
    ScrollerSnapPointSortPredicate SortPredicate();
    void DetermineActualApplicableZone(SnapPointBase* previousSnapPoint, SnapPointBase* nextSnapPoint);
    double Influence(double edgeOfMidpoint);
    void Combine(winrt::SnapPointBase const& snapPoint);
    double Evaluate(double value);

    double ActualStart();
    void ActualStart(double start);
    double ActualEnd();
    void ActualEnd(double end);

private:
    double DetermineFirstRepeatedSnapPointValue();
    double DetermineMinActualApplicableZone(SnapPointBase* previousSnapPoint);
    double DetermineMaxActualApplicableZone(SnapPointBase* nextSnapPoint);
    void ValidateConstructorParameters(
        double offset,
        double interval,
        double start,
        double end,
        double applicableRange,
        bool applicableRangeTypeToo);

    double m_offset{ 0.0f };
    double m_interval{ 0.0f };
    double m_specifiedStart{ 0.0f };
    double m_actualStart{ 0.0f };
    double m_specifiedEnd{ 0.0f };
    double m_actualEnd{ 0.0f };
};

struct winrtProjectionComparator
{
    inline bool operator()(winrt::SnapPointBase left, winrt::SnapPointBase right) const
    {
        return *winrt::get_self<SnapPointBase>(left) < winrt::get_self<SnapPointBase>(right);
    }
};
