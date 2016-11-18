class Action:
    """ Represents an action in the model """

    def __init__(self, row_group_start, row_group_end, row, model):
        """ Initialize
        :param row_group_start: Start index of the row group in the matrix
        :param row_group_end: End index of the row group in the matrix
        :param row: Index of the corresponding row in the matrix
        :param model: Corresponding model
        """
        self.row_group_start = row_group_start
        self.row_group_end = row_group_end
        self.row = row - 1
        self.model = model
        assert row >= -1 and row + row_group_start <= row_group_end

    def __iter__(self):
        return self

    def __next__(self):
        if self.row + self.row_group_start >= self.row_group_end - 1:
            raise StopIteration
        else:
            self.row += 1
            return self

    def __str__(self):
        return "{}".format(self.row)

    def transitions(self):
        """ Get transitions associated with the action
        :return List of tranistions
        """
        row = self.row_group_start + self.row
        #return self.model.transition_matrix().get_row(self.row_group_start + self.row)
        return self.model.transition_matrix.row_iter(row, row)
