# Mostly written by chatgpt!!!
class TextCollection:
    def __init__(self, loc, ax, texts, **kwargs):
        """
        Initializes the TextCollection with a Matplotlib Axes object and a list of initial texts.
        
        Parameters:
        - ax: Matplotlib Axes object
        - texts: list of strings, initial texts for the annotations
        - **kwargs: additional keyword arguments for text properties
        """
        self.ax = ax
        ax.set_axis_off()

        self.text_objects = [ax.text(loc[0],loc[1], texts[0], color="black", fontsize = "large", fontweight="bold")]
        for text in texts[1:]:
            text_obj = ax.annotate(text, xy=(1,0), **kwargs, xycoords= self.text_objects[-1], color="black", verticalalignment="bottom", fontsize= "large", fontweight = "bold")
            self.text_objects.append(text_obj)

    def get_visible(self):
        return self.text_objects[0].get_visible()
    def set_visible(self, visible):
        """
        Sets the visibility of all text annotations in the collection.
        
        Parameters:
        - visible: boolean, True to make text visible, False to hide text
        """
        for text_obj in self.text_objects:
            text_obj.set_visible(visible)
        #self.ax.figure.canvas.draw_idle()
        #dont update, we'll update all at once

    def set_texts(self, texts, colors):
        """
        Updates the text and color of all annotations in the collection.
        
        Parameters:
        - texts: list of strings, new texts for the annotations
        - colors: list of colors for each text annotation
        """
        if len(texts) != len(self.text_objects) or len(colors) != len(self.text_objects):
            raise ValueError("Number of texts and colors should match the number of text annotations.")
        
        for text_obj, new_text, color in zip(self.text_objects, texts, colors):
            text_obj.set_text(new_text)
            text_obj.set_color(color)
        #self.ax.figure.canvas.draw_idle()

    def set_text_at_index(self, index, text, color):
        """
        Updates the text and color of the annotation at the specified index.
        
        Parameters:
        - index: integer, index of the text annotation to update
        - text: string, new text for the annotation
        - color: color for the annotation
        """
        text_obj = self.text_objects[index]
        text_obj.set_text(text)
        text_obj.set_color(color)
        #self.ax.figure.canvas.draw_idle()