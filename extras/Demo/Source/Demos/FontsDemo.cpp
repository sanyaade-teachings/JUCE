/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-12 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "../JuceDemoHeader.h"


//==============================================================================
class FontsDemo  : public Component,
                   private ListBoxModel,
                   private Slider::Listener,
                   private Button::Listener,
                   private ComboBox::Listener
{
public:
    FontsDemo()
        : heightLabel (String::empty, "Height:"),
          kerningLabel (String::empty, "Kerning:"),
          scaleLabel  (String::empty, "Scale:"),
          styleLabel ("Style"),
          boldToggle ("Bold"),
          italicToggle ("Italic")
    {
        setOpaque (true);

        addAndMakeVisible (&listBox);
        addAndMakeVisible (&demoTextBox);
        addAndMakeVisible (&heightSlider);
        addAndMakeVisible (&heightLabel);
        addAndMakeVisible (&kerningLabel);
        addAndMakeVisible (&kerningSlider);
        addAndMakeVisible (&scaleLabel);
        addAndMakeVisible (&scaleSlider);
        addAndMakeVisible (&boldToggle);
        addAndMakeVisible (&italicToggle);
        addAndMakeVisible (&styleBox);

        kerningLabel.attachToComponent (&kerningSlider, true);
        heightLabel.attachToComponent (&heightSlider, true);
        scaleLabel.attachToComponent (&scaleSlider, true);
        styleLabel.attachToComponent (&styleBox, true);

        heightSlider.addListener (this);
        kerningSlider.addListener (this);
        scaleSlider.addListener (this);
        boldToggle.addListener (this);
        italicToggle.addListener (this);
        styleBox.addListener (this);

        Font::findFonts (fonts);   // Generate the list of fonts

        listBox.setRowHeight (20);
        listBox.setModel (this);   // Tell the listbox where to get its data model

        heightSlider.setRange (3.0, 150.0, 0.01);
        scaleSlider.setRange (0.2, 3.0, 0.01);
        kerningSlider.setRange (-2.0, 2.0, 0.01);

        scaleSlider.setValue (1.0);   // Set some initial values for the sliders.
        heightSlider.setValue (20.0);
        kerningSlider.setValue (0);

        // set up the layout and resizer bars..
        verticalLayout.setItemLayout (0, -0.2, -0.8, -0.35); // width of the font list must be
                                                             // between 20% and 80%, preferably 50%
        verticalLayout.setItemLayout (1, 8, 8, 8);           // the vertical divider drag-bar thing is always 8 pixels wide
        verticalLayout.setItemLayout (2, 150, -1.0, -0.65);  // the components on the right must be
                                                             // at least 150 pixels wide, preferably 50% of the total width

        verticalDividerBar = new StretchableLayoutResizerBar (&verticalLayout, 1, true);
        addAndMakeVisible (verticalDividerBar);

        // ..and pick a random font to select intially
        listBox.selectRow (Random::getSystemRandom().nextInt (fonts.size()));

        demoTextBox.setMultiLine (true);
        demoTextBox.setReturnKeyStartsNewLine (true);
        demoTextBox.setText ("The Quick Brown Fox Jumped Over The Lazy Dog\n\n"
                             "Aa Bb Cc Dd Ee Ff Gg Hh Ii\n"
                             "Jj Kk Ll Mm Nn Oo Pp Qq Rr\n"
                             "Ss Tt Uu Vv Ww Xx Yy Zz\n"
                             "0123456789");

        demoTextBox.setCaretPosition (0);
    }

    //==============================================================================
    void paint (Graphics& g)
    {
        fillBrushedAluminiumBackground (g);
    }

    void resized()
    {
        Rectangle<int> r (getLocalBounds().reduced (5));

        // lay out the list box and vertical divider..
        Component* vcomps[] = { &listBox, verticalDividerBar, nullptr };

        verticalLayout.layOutComponents (vcomps, 3,
                                         r.getX(), r.getY(), r.getWidth(), r.getHeight(),
                                         false,     // lay out side-by-side
                                         true);     // resize the components' heights as well as widths


        r.removeFromLeft (verticalDividerBar->getRight());

        styleBox.setBounds (r.removeFromBottom (26));
        r.removeFromBottom (8);

        const int labelWidth = 60;

        Rectangle<int> row (r.removeFromBottom (30));
        row.removeFromLeft (labelWidth);
        boldToggle.setBounds (row.removeFromLeft (row.getWidth() / 2));
        italicToggle.setBounds (row);

        r.removeFromBottom (8);
        scaleSlider.setBounds (r.removeFromBottom (30).withTrimmedLeft (labelWidth));
        r.removeFromBottom (8);
        kerningSlider.setBounds (r.removeFromBottom (30).withTrimmedLeft (labelWidth));
        r.removeFromBottom (8);
        heightSlider.setBounds (r.removeFromBottom (30).withTrimmedLeft (labelWidth));
        r.removeFromBottom (8);
        demoTextBox.setBounds (r);
    }

    void sliderValueChanged (Slider* sliderThatWasMoved)
    {
        if (sliderThatWasMoved == &heightSlider)            refreshPreviewBoxFont();
        else if (sliderThatWasMoved == &kerningSlider)      refreshPreviewBoxFont();
        else if (sliderThatWasMoved == &scaleSlider)        refreshPreviewBoxFont();
    }

    void buttonClicked (Button* buttonThatWasClicked)
    {
        if (buttonThatWasClicked == &boldToggle)            refreshPreviewBoxFont();
        else if (buttonThatWasClicked == &italicToggle)     refreshPreviewBoxFont();
    }

    // The following methods implement the ListBoxModel virtual methods:
    int getNumRows()
    {
        return fonts.size();
    }

    void paintListBoxItem (int rowNumber, Graphics& g,
                           int width, int height, bool rowIsSelected)
    {
        if (rowIsSelected)
            g.fillAll (Colours::lightblue);

        Font font (fonts [rowNumber]);

        AttributedString s;
        s.setWordWrap (AttributedString::none);
        s.setJustification (Justification::centredLeft);
        s.append (font.getTypefaceName(), font.withHeight (height * 0.8f), Colours::black);
        s.append ("   " + font.getTypefaceName(), Font (height * 0.5f, Font::italic), Colours::grey);

        s.draw (g, Rectangle<float> (4.0f, 0.0f, width - 5.0f, (float) height));
    }

    void selectedRowsChanged (int /*lastRowselected*/)
    {
        refreshPreviewBoxFont();
    }

private:
    Array<Font> fonts;
    StringArray currentStyleList;

    ListBox listBox;
    TextEditor demoTextBox;
    Label heightLabel, kerningLabel, scaleLabel, styleLabel;
    Slider heightSlider, kerningSlider, scaleSlider;
    ToggleButton boldToggle, italicToggle;
    ComboBox styleBox;

    StretchableLayoutManager verticalLayout;
    ScopedPointer<StretchableLayoutResizerBar> verticalDividerBar;

    void refreshPreviewBoxFont()
    {
        const bool bold = boldToggle.getToggleState();
        const bool italic = italicToggle.getToggleState();
        const bool useStyle = ! (bold || italic);

        Font font (fonts [listBox.getSelectedRow()]);

        font.setHeight ((float) heightSlider.getValue());
        font.setBold (bold);
        font.setItalic (italic);
        font.setExtraKerningFactor ((float) kerningSlider.getValue());
        font.setHorizontalScale ((float) scaleSlider.getValue());

        updateStylesList (font);

        styleBox.setEnabled (useStyle);

        if (useStyle)
            font.setTypefaceStyle (styleBox.getText());

        demoTextBox.applyFontToAllText (font);
    }

    void updateStylesList (const Font& newFont)
    {
        const StringArray newStyles (newFont.getAvailableStyles());

        if (newStyles != currentStyleList)
        {
            currentStyleList = newStyles;

            styleBox.clear();
            styleBox.addItemList (newStyles, 1);
            styleBox.setSelectedItemIndex (0);
        }
    }

    void comboBoxChanged (ComboBox* box) override
    {
        if (box == &styleBox)
            refreshPreviewBoxFont();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FontsDemo)
};


// This static object will register this demo type in a global list of demos..
static JuceDemoType<FontsDemo> demo ("20 Graphics: Fonts");
