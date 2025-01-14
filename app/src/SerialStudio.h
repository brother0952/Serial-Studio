/*
 * Copyright (c) 2024 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

// Qt headers must be included first for Q_OS_WIN macro
#include <QObject>
#include <QVector>

// Windows API headers must be included first
#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define SECURITY_WIN32
#include <windows.h>
#include <sddl.h>
#include <security.h>
#endif

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

#include "JSON/Group.h"
#include "JSON/Dataset.h"

/**
 * @typedef PlotDataX
 * @brief Represents the unique X-axis data points for a plot.
 *
 * The X-axis data points are stored as a set of unique `qreal` values.
 * This ensures that each X value is distinct, which is essential for correct
 * rendering of the plot. The set is inherently ordered in ascending order.
 */
typedef QVector<qreal> PlotDataX;

/**
 * @typedef PlotDataY
 * @brief Represents the Y-axis data points for a single curve.
 *
 * The Y-axis data points are stored as a vector of `qreal` values.
 * Unlike X-axis data, Y values can have duplicates and are directly
 * mapped to the corresponding X values during plotting.
 */
typedef QVector<qreal> PlotDataY;

/**
 * @typedef MultiPlotDataY
 * @brief Represents Y-axis data for multiple curves in a multiplot.
 *
 * A vector of `PlotDataY` structures, where each element represents
 * the Y-axis data for one curve in a multiplot widget. This allows
 * managing multiple curves independently.
 */
typedef QVector<PlotDataY> MultiPlotDataY;

/**
 * @typedef LineSeries
 * @brief Represents a paired series of X-axis and Y-axis data for a plot.
 *
 * The `LineSeries` type is defined as a `QPair` containing:
 * - A pointer to `PlotDataX`, which holds the unique X-axis values.
 * - A pointer to `PlotDataY`, which holds the Y-axis values.
 *
 * This type simplifies data processing by tightly coupling the related X and Y
 * data for a plot, ensuring that they are always accessed and managed together.
 */
typedef struct
{
  PlotDataX *x;
  PlotDataY *y;
} LineSeries;

/**
 * @typedef MultiLineSeries
 */
typedef struct
{
  PlotDataX *x;
  QList<PlotDataY> y;
} MultiLineSeries;

/**
 * @class SerialStudio
 * @brief A central utility class for managing data visualization and decoding
 * logic.
 *
 * The `SerialStudio` class provides a set of tools and enums for working with
 * continuous data streams, decoding strategies, and visualization elements. It
 * serves as a foundation for configuring, parsing, and visualizing incoming
 * data from various sources such as serial ports, network sockets, and
 * Bluetooth LE devices.
 *
 * Key Features:
 * - **Decoding Methods**: Support for PlainText, Hexadecimal, and Base64
 *   decoding.
 * - **Frame Detection**: Configurable methods for detecting data frames in
 *   streams, including end-delimiter-only and start-and-end-delimiter
 * strategies.
 * - **Operation Modes**: Different approaches for building dashboards, such as
 *   reading from project files, JSON devices, or quick plots.
 * - **Visualization Widgets**: Enumerations for different widget types
 *   available for groups, datasets, and dashboards.
 * - **Utility Functions**: Static functions for managing widgets, colors, and
 *   icon/title associations in dashboards.
 *
 * @details
 * This class is highly modular, enabling developers to work with a wide variety
 * of visualization tools and data handling methods. It supports the
 * customization of dashboard components and datasets to suit specific
 * application needs.
 *
 * @enums
 * - **DecoderMethod**: Defines methods for decoding data streams.
 * - **FrameDetection**: Configures strategies for detecting frames in data
 *                       streams.
 * - **OperationMode**: Specifies methods for building dashboards.
 * - **BusType**: Enumerates the available data sources.
 * - **GroupWidget**: Lists visualization widget types for groups.
 * - **DatasetWidget**: Lists visualization widget types for datasets.
 * - **DashboardWidget**: Enumerates visualization widget types for dashboards.
 * - **DatasetOption**: Bit-flag options for dataset configurations.
 */
class SerialStudio : public QObject
{
  Q_OBJECT

public:
  /**
   * @enum DecoderMethod
   * @brief Specifies the available methods for decoding data from a continuous
   * stream.
   *
   * This enum defines different decoding strategies that can be applied to
   * interpret incoming data. Each value represents a specific decoding format.
   */
  enum DecoderMethod
  {
    PlainText,   /**< Standard decoding, interprets data as plain text. */
    Hexadecimal, /**< Decodes data assuming a hexadecimal-encoded format. */
    Base64       /**< Decodes data assuming a Base64-encoded format. */
  };
  Q_ENUM(DecoderMethod)

  /**
   * @brief Specifies the method used to detect data frames within a continuous
   *        stream.
   *
   * This enum is used to define the strategy for identifying the start and end
   * of a data frame in a continuous stream. Each value represents a specific
   * detection method.
   */
  enum FrameDetection
  {
    EndDelimiterOnly,     /**< Detects frames based only on an end delimiter. */
    StartAndEndDelimiter, /**< Detects frames based on both start and end
                               delimiters. */
    NoDelimiters          /**< Disables frame detection and processes incoming
                               data directly */
  };
  Q_ENUM(FrameDetection)

  /**
   * @enum OperationMode
   * @brief Specifies the method used to construct a dashboard.
   *
   * The `OperationMode` enum defines the strategies for building dashboards
   * based on the data source or project configuration. Each mode represents
   * a different approach to interpreting and visualizing the incoming data.
   */
  enum OperationMode
  {
    ProjectFile, /**< Builds the dashboard using a predefined project file. */
    DeviceSendsJSON, /**< Builds the dashboard from device-sent JSON. */
    QuickPlot,       /**< Quick and simple data plotting mode. */
  };
  Q_ENUM(OperationMode)

  /**
   * @enum BusType
   * @brief Enumerates the available data sources for communication.
   *
   * The `BusType` enum defines the supported communication protocols or data
   * sources used for receiving data streams. Each bus type corresponds to a
   * specific hardware or network interface.
   */
  enum class BusType
  {
    Serial,     /**< Serial port communication. */
    Network,    /**< Network socket communication. */
    BluetoothLE /**< Bluetooth Low Energy communication. */
  };
  Q_ENUM(BusType)

  /**
   * @enum AxisVisibility
   * @brief Specifies the visibility options for axes in a plot or
   * visualization.
   *
   * The `AxisVisibility` enum defines bitwise flags to control the visibility
   * of the X and Y axes in a plot or visualization. These flags allow
   * fine-grained control over which axes are displayed.
   */
  enum AxisVisibility
  {
    AxisXY = 0b11,        /**< Both X and Y axes are visible. */
    AxisX = 0b01,         /**< Only the X axis is visible. */
    AxisY = 0b10,         /**< Only the Y axis is visible. */
    NoAxesVisible = 0b00, /**< Neither axis is visible. */
  };
  Q_ENUM(AxisVisibility)

  /**
   * @brief Enum representing the different widget types available for groups.
   */
  enum GroupWidget
  {
    DataGrid,
    Accelerometer,
    Gyroscope,
    GPS,
    MultiPlot,
    NoGroupWidget,
  };
  Q_ENUM(GroupWidget)

  /**
   * @brief Enum representing the different widget types available for datasets.
   */
  enum DatasetWidget
  {
    Bar,
    Gauge,
    Compass,
    NoDatasetWidget
  };
  Q_ENUM(DatasetWidget)

  /**
   * @brief Enum representing the different widget types available for the
   *        dashboard.
   */
  enum DashboardWidget
  {
    DashboardDataGrid,
    DashboardMultiPlot,
    DashboardAccelerometer,
    DashboardGyroscope,
    DashboardGPS,
    DashboardFFT,
    DashboardLED,
    DashboardPlot,
    DashboardBar,
    DashboardGauge,
    DashboardCompass,
    DashboardNoWidget
  };
  Q_ENUM(DashboardWidget)

  /**
   * @brief Enum representing the options available for datasets.
   *
   * These options are represented as bit flags, allowing for combinations of
   * multiple options.
   */
  // clang-format off
  enum DatasetOption
  {
    DatasetGeneric = 0b00000000,
    DatasetPlot    = 0b00000001,
    DatasetFFT     = 0b00000010,
    DatasetBar     = 0b00000100,
    DatasetGauge   = 0b00001000,
    DatasetCompass = 0b00010000,
    DatasetLED     = 0b00100000,
  };
  Q_ENUM(DatasetOption)
  // clang-format on

  //
  // Dashboard logic
  //
  // clang-format off
  [[nodiscard]] static bool isGroupWidget(const DashboardWidget widget);
  [[nodiscard]] static bool isDatasetWidget(const DashboardWidget widget);
  [[nodiscard]] static QString dashboardWidgetIcon(const DashboardWidget w);
  [[nodiscard]] static QString dashboardWidgetTitle(const DashboardWidget w);
  [[nodiscard]] static DashboardWidget getDashboardWidget(const JSON::Group& group);
  [[nodiscard]] static QList<DashboardWidget> getDashboardWidgets(const JSON::Dataset& dataset);
  // clang-format on

  //
  // Parsing & project model logic
  //
  [[nodiscard]] static QString groupWidgetId(const GroupWidget widget);
  [[nodiscard]] static GroupWidget groupWidgetFromId(const QString &id);
  [[nodiscard]] static QString datasetWidgetId(const DatasetWidget widget);
  [[nodiscard]] static DatasetWidget datasetWidgetFromId(const QString &id);

  //
  // Utility functions
  //
  [[nodiscard]] static QString getDatasetColor(const int index);

  static bool isRunningAsAdmin() {
    #ifdef Q_OS_WIN
        BOOL isAdmin = FALSE;
        PSID adminGroup;
        SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
        
        if (AllocateAndInitializeSid(&ntAuthority, 2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0,
            &adminGroup)) {
            CheckTokenMembership(NULL, adminGroup, &isAdmin);
            FreeSid(adminGroup);
        }
        return isAdmin;
    #else
        return geteuid() == 0;
    #endif
  }
};
