﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using Common;
using MUXControlsTestApp.Utilities;
using System.Linq;
using System.Threading;
using Windows.Foundation.Metadata;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using MUXControls.TestAppUtils;
using PlatformConfiguration = Common.PlatformConfiguration;
using OSVersion = Common.OSVersion;
using System.Collections.Generic;

#if USING_TAEF
using WEX.TestExecution;
using WEX.TestExecution.Markup;
using WEX.Logging.Interop;
#else
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.VisualStudio.TestTools.UnitTesting.Logging;
#endif



namespace Windows.UI.Xaml.Tests.MUXControls.ApiTests
{
    [TestClass]
    public class CommonStylesVisualTreeTests: VisualTreeTestBase
    {
        [TestMethod]
        public void VerifyVisualTreeForAppBarAndAppBarToggleButton()
        {
            if (PlatformConfiguration.IsOSVersionLessThan(OSVersion.Redstone5))
            {
                // master file not ready until VisualTreeDumper is stable
                return;
            }

            var xaml = @"<Grid Width='400' Height='400' xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'> 
                            <StackPanel>
                                <AppBarButton Icon='Accept' Label='Accept'/>
                                <AppBarToggleButton Icon='Dislike' Label='Dislike'/>
                            </StackPanel>
                       </Grid>";
            VerifyVisualTree(xaml);
        }

        [TestMethod]
        public void VerifyVisualTreeExampleLoadAndVerifyForAllThemes()
        {
            if (PlatformConfiguration.IsOSVersionLessThan(OSVersion.Redstone5))
            {
                return;
            }

            var xaml = @"<Grid Width='400' Height='400' xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'> 
                       </Grid>";
            VerifyVisualTree(xaml);
        }

        [TestMethod]
        public void VerifyVisualTreeExampleLoadAndVerifyForDarkThemeWithCustomName()
        {
            if (PlatformConfiguration.IsOSVersionLessThan(OSVersion.Redstone5))
            {
                return;
            }

            var xaml = @"<Grid Width='400' Height='400' xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'> 
                       </Grid>";
            UIElement root = SetupVisualTree(xaml);
            RunOnUIThread.Execute(() =>
            {
                (root as FrameworkElement).RequestedTheme = ElementTheme.Dark;
            });
            VerifyVisualTree(root, "CustomName");
        }

        [TestMethod]
        public void VerifyVisualTreeExampleForLightTheme()
        {
            if (PlatformConfiguration.IsOSVersionLessThan(OSVersion.Redstone5))
            {
                return;
            }

            var xaml = @"<Grid Width='400' Height='400' xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'> 
                       </Grid>";
            UIElement root = SetupVisualTree(xaml);
            VerifyVisualTree(root, Theme.Light);
        }

        [TestMethod]
        public void VerifyVisualTreeExampleWithCustomerFilter()
        {
            if (PlatformConfiguration.IsOSVersionLessThan(OSVersion.Redstone5))
            {
                return;
            }

            var xaml = @"<Grid Width='400' Height='400' xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'> 
                        <TextBlock Text='Abc' />
                       </Grid>";

            VisualTreeDumpFilter = new CustomFilter();
            VerifyVisualTree(xaml);
        }

        [TestMethod]
        public void VerifyVisualTreeExampleWithCustomerPropertyValueTranslator()
        {
            if (PlatformConfiguration.IsOSVersionLessThan(OSVersion.Redstone5))
            {
                return;
            }

            var xaml = @"<Grid Width='400' Height='400' xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'> 
                            <TextBlock Text='Abc' />
                       </Grid>";

            PropertyValueTranslator = new CustTranslate();
            VerifyVisualTree(xaml);
        }

        class CustomFilter : VisualTreeDumper.IFilter
        {

            private static readonly string[] _propertyNamePostfixBlackList = new string[] { "Property", "Transitions", "Template", "Style", "Selector" };

            private static readonly string[] _propertyNameBlackList = new string[] { "Interactions", "ColumnDefinitions", "RowDefinitions",
            "Children", "Resources", "Transitions", "Dispatcher", "TemplateSettings", "ContentTemplate", "ContentTransitions",
            "ContentTemplateSelector", "Content", "ContentTemplateRoot", "XYFocusUp", "XYFocusRight", "XYFocusLeft", "Parent",
            "Triggers", "RequestedTheme", "XamlRoot", "IsLoaded", "BaseUri", "Resources"};

            private static readonly Dictionary<string, string> _knownPropertyValueDict = new Dictionary<string, string> {
                {"Padding", "0,0,0,0"},
                {"IsTabStop", "False" },
                {"IsEnabled", "True"},
                {"IsLoaded", "True" },
                {"HorizontalContentAlignment", "Center" },
                {"FontSize", "14" },
                {"TabIndex", "2147483647" },
                {"AllowFocusWhenDisabled", "False" },
                {"CharacterSpacing", "0" },
                {"BorderThickness", "0,0,0,0"},
                {"FocusState", "Unfocused"},
                {"IsTextScaleFactorEnabled", "True" },
                {"UseSystemFocusVisuals","False" },
                {"RequiresPointer","Never" },
                {"IsFocusEngagementEnabled","False" },
                {"IsFocusEngaged","False" },
                {"ElementSoundMode","Default" },
                {"CornerRadius","0,0,0,0" },
                {"BackgroundSizing","InnerBorderEdge" },
                {"Width","NaN" },
                {"Name","" },
                {"MinWidth","0" },
                {"MinHeight","0" },
                {"MaxWidth","∞" },
                {"MaxHeight","∞" },
                {"Margin","0,0,0,0" },
                {"Language","en-US" },
                {"HorizontalAlignment","Stretch" },
                {"Height","NaN" },
                {"FlowDirection","LeftToRight" },
                {"RequestedTheme","Default" },
                {"FocusVisualSecondaryThickness","1,1,1,1" },
                {"FocusVisualPrimaryThickness","2,2,2,2" },
                {"FocusVisualMargin","0,0,0,0" },
                {"AllowFocusOnInteraction","True" },
                {"Visibility","Visible" },
                {"UseLayoutRounding","True" },
                {"RenderTransformOrigin","0,0" },
                {"AllowDrop","False" },
                {"Opacity","1" },
                {"ManipulationMode","System" },
                {"IsTapEnabled","True" },
                {"IsRightTapEnabled","True" },
                {"IsHoldingEnabled","True" },
                {"IsHitTestVisible","True" },
                {"IsDoubleTapEnabled","True" },
                {"CanDrag","False" },
                {"IsAccessKeyScope","False" },
                {"ExitDisplayModeOnAccessKeyInvoked","True" },
                {"AccessKey","" },
                {"KeyTipHorizontalOffset","0" },
                {"XYFocusRightNavigationStrategy","Auto" },
                {"HighContrastAdjustment","Application" },
                {"TabFocusNavigation","Local" },
                {"XYFocusUpNavigationStrategy","Auto" },
                {"XYFocusLeftNavigationStrategy","Auto" },
                {"XYFocusKeyboardNavigation","Auto" },
                {"XYFocusDownNavigationStrategy","Auto" },
                {"KeyboardAcceleratorPlacementMode","Auto" },
                {"CanBeScrollAnchor","False" },
                {"Translation","<0, 0, 0>" },
                {"Scale","<1, 1, 1>" },
                {"RotationAxis","<0, 0, 1>" },
                {"CenterPoint","<0, 0, 0>" },
                {"Rotation","0" },
                {"TransformMatrix","{ {M11:1 M12:0 M13:0 M14:0} {M21:0 M22:1 M23:0 M24:0} {M31:0 M32:0 M33:1 M34:0} {M41:0 M42:0 M43:0 M44:1} }"},
            };

            public virtual bool IsKnownProperty(string propertyName, string value)
            {
                string v = _knownPropertyValueDict.ContainsKey(propertyName) ? _knownPropertyValueDict[propertyName] : VisualTreeDumper.NULL;
                return v.Equals(value);
            }

            public virtual bool ShouldLogElement(string elementName)
            {
                return true;
            }

            public virtual bool ShouldLogProperty(string propertyName)
            {
                return (_propertyNamePostfixBlackList.Where(item => propertyName.EndsWith(item)).Count()) == 0 &&
                    !_propertyNameBlackList.Contains(propertyName);
            }
        }


        class CustTranslate : VisualTreeDumper.DefaultPropertyValueTranslator // Add prefix MyValue to all Value
        {
            public override string PropertyValueToString(string propertyName, object value)
            {
                return "MyValue" + base.PropertyValueToString(propertyName, value);
            }
        }
    }
}
