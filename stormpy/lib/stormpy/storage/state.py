from . import action

class State:
    """ Represents a state in the model """

    def __init__(self, id, model):
        """ Initialize
        :param id: Id of the state
        :param model: Corresponding model
        """
        self.id = id - 1
        self.model = model

    def __iter__(self):
        return self

    def __next__(self):
        if self.id >= self.model.nr_states - 1:
            raise StopIteration
        else:
            self.id += 1
            return self

    def __str__(self):
        return "{}".format(self.id)

    def actions(self):
        """ Get actions associated with the state
        :return List of actions
        """
        row_group_indices = self.model.transition_matrix._row_group_indices
        start = row_group_indices[self.id]
        end = row_group_indices[self.id+1]
        return action.Action(start, end, 0, self.model)
