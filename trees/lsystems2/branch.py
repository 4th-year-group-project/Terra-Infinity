class Branch: 
    def __init__(self, pos, heading, left, up, width):
        self.pos = pos
        self.heading = heading
        self.left = left
        self.up = up
        self.width = width
        self.poss = [pos]
        self.headings = [heading]
        self.lefts = [left]
        self.ups = [up]
        self.widths = [width]
    
    def add_point(self, pos, heading, left, up, width):
        self.poss.append(pos)
        self.headings.append(heading)
        self.lefts.append(left)
        self.ups.append(up)
        self.widths.append(width)

    def compute_bezier(self):
        # Compute the Bezier curve using the points in self.poss
        # This is a placeholder for the actual Bezier computation
        # You can use the De Casteljau's algorithm or any other method
        # to compute the Bezier curve from the control points.
        # For now, let's just return the points as a straight line
        return self.poss
    
    def copy(self):
        y = Branch(self.pos, self.heading, self.left, self.up, self.width)
        y.poss = self.poss.copy()
        y.headings = self.headings.copy()
        y.lefts = self.lefts.copy()
        y.ups = self.ups.copy()
        y.widths = self.widths.copy()
        return y
    
    def get_mesh(self): 
        return [],[]
    
    def __str__(self):
        return f"Branch {self.pos}"