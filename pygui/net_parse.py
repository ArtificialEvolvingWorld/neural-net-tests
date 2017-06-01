#!/usr/bin/env python3

import os
import sys

import pyparsing as p

sys.path.append(os.path.join(os.path.dirname(__file__),
                             os.path.pardir,
                             'lib'))
import pyneat

class NetTemplate:
    def __init__(self, inputs, outputs,
                 connections, activation_funcs):
        self.inputs = inputs
        self.outputs = outputs
        self.connections = connections
        self.activation_funcs = activation_funcs

        self._make_node_indices()


    def _make_node_indices(self):

        self.node_indices = {}
        self.hidden_nodes = []

        index = 0
        def add_it(name, hidden=False):
            nonlocal index
            if name not in self.node_indices:
                self.node_indices[name] = index
                index += 1

                if hidden:
                    self.hidden_nodes.append(name)

        add_it('bias')

        for input_node in self.inputs:
            add_it(input_node)

        for output_node in self.outputs:
            add_it(output_node)

        for conn in self.connections:
            add_it(conn.origin, True)
            add_it(conn.dest, True)


    @property
    def num_inputs(self):
        return len(self.inputs)

    @property
    def num_outputs(self):
        return len(self.outputs)

    @property
    def num_hidden(self):
        return len(self.node_indices) - self.num_inputs - self.num_outputs - 1

    def make_genome(self):
        genome = pyneat.Genome()

        # Add all nodes
        genome.AddNode(pyneat.NodeType.Bias)
        for input_node in self.inputs:
            genome.AddNode(pyneat.NodeType.Input)
        for output_node in self.outputs:
            func = self.get_activation_func(output_node)
            genome.AddNode(pyneat.NodeType.Output, func)
        for hidden_node in self.hidden_nodes:
            func = self.get_activation_func(hidden_node)
            genome.AddNode(pyneat.NodeType.Hidden, func)

        # Add all connections
        for conn in self.connections:
            genome.AddConnection(self.node_indices[conn.origin],
                                 self.node_indices[conn.dest],
                                 True, 1.0)

        return genome

    def get_activation_func(self, node):
        if node in self.activation_funcs:
            return self.activation_funcs[node]
        else:
            return pyneat.ActivationFunction.Sigmoid


class Connection:
    def __init__(self, origin, dest, weight=1.0,
                 is_recurrent=False):
        self.origin = origin
        self.dest = dest
        self.weight = weight
        self.is_recurrent = is_recurrent

    def __repr__(self):
        return '{} {} {} (weight={})'.format(
            self.origin,
            '-R->' if self.is_recurrent else '-->',
            self.dest,
            self.weight)


def check_not_bias(pstr, loc, m):
    name = m[0]
    if name.lower() == 'bias':
        raise p.ParseFatalException(pstr, loc,
                                    msg='Bias node cannot be used here')

def normalize_bias(pstr, loc, m):
    if m[0].lower() == 'bias':
        return 'bias'
    else:
        return m

identifier = p.Word(p.alphanums + '_').setParseAction(normalize_bias)
not_bias_identifier = identifier.copy().setParseAction(check_not_bias)

integer = p.Combine(p.Optional(p.Literal('-') ^ p.Literal('+')) +
                    p.Word(p.nums)
).setParseAction(lambda t:int(t[0]))

double = p.Combine(integer +
                   p.Optional('.' + p.Word(p.nums)) +
                   p.Optional('e' + integer)
).setParseAction(lambda t:float(t[0]))

def make_connection(pstr, loc, m):
    weight = m['weight'] if 'weight' in m else 1.0
    is_recurrent = m['type'] == '-R->'
    origin = m['origin']
    dest = m['dest']

    if origin.lower() == 'bias' and is_recurrent:
        raise p.ParseFatalException(pstr, loc,
                                    msg='Bias node cannot be origin of recurrent connection')

    return Connection(origin = m['origin'],
                      dest = m['dest'],
                      weight = weight,
                      is_recurrent = is_recurrent,
                      )

def decorate(marker):
    def decoratorParseAction(pstr, loc, m):
        return (marker, m)
    return decoratorParseAction

connection = (identifier('origin') +
              (p.Literal('-->') ^ p.CaselessLiteral('-R->'))('type') +
              not_bias_identifier('dest') +
              p.Optional(p.Literal('(') +
                         (p.CaselessLiteral('w') ^ p.CaselessLiteral('weight')) +
                         p.Literal('=') +
                         double('weight') +
                         p.Literal(')'))
              ).addParseAction(make_connection).addParseAction(decorate('conn'))

input_nodes = ((p.CaselessLiteral('Input') +
                p.Literal(':')).suppress() +
               p.delimitedList(not_bias_identifier)).addParseAction(decorate('inputs'))

output_nodes = ((p.CaselessLiteral('Output') +
                 p.Literal(':')).suppress() +
                p.delimitedList(not_bias_identifier)).addParseAction(decorate('outputs'))


activation_func = p.oneOf([t for t in dir(pyneat.ActivationFunction)
                           if not t.startswith('_')], caseless=True).addParseAction(
                                   lambda m:getattr(pyneat.ActivationFunction,m[0]))
activation_listing = (activation_func +
                      p.Literal(':').suppress() +
                      p.delimitedList(not_bias_identifier)).addParseAction(decorate('activation'))



def make_net(pstr, loc, m):
    connections = []
    inputs = []
    outputs = []
    activation_funcs = {}

    for tag,obj in m[0]:
        if tag=='conn':
            connections.append(obj[0])
        elif tag=='inputs':
            inputs.extend(obj)
        elif tag=='outputs':
            outputs.extend(obj)
        elif tag=='activation':
            func = obj[0]
            for node_name in obj[1:]:
                if node_name in activation_funcs:
                    msg = ('Repeated activation func for node "{}": '
                           '{} and {}').format(
                               node_name, func, activation_funcs[node_name])
                    raise p.ParseFatalException(pstr, loc, msg=msg)

                activation_funcs[node_name] = func

    for node_name in activation_funcs:
        if node_name in inputs:
            msg = ('Node "{}" is an input, '
                   'and cannot have an activation function').format(node_name)
            raise p.ParseFatalException(pstr, loc, msg=msg)

    for conn in connections:
        if conn.dest in inputs:
            msg = ('Input nodes cannot be destination of connections'
                   ' ({})'.format(conn))
            raise p.ParseFatalException(pstr, loc, msg=msg)

    return NetTemplate(inputs, outputs, connections, activation_funcs)

net = p.Group(p.OneOrMore(input_nodes ^
                          output_nodes ^
                          connection ^
                          activation_listing)).ignore(p.pythonStyleComment).setParseAction(make_net)


example_id = 'asdf'
example_conn = 'asdf --> qwer'
example_input = 'Input: a, b, c'
example_activation = 'Tanh: asdf'

example = """
# Python-style comments are used.

# Input and output nodes are named in a list.
# If multiple Input sections exist, all will be concatenated.
# Node names can consist of any alphanumeric characters and _,
# and are case-sensitive.
# "Bias" is reserved, and cannot be used for user-defined nodes.
Input: x, v
Output: F

# Connections are specified with an arrow from origin to destination.
x --> der

# By default, the connection weight is 1.0
# To specify a weight, place it in parantheses after the connection.
x --> F (w=3.0)
der --> F (weight=1.5)

# Recurrent connections are specified by placing an R in the connection
der -R-> der

# The bias node can be connected to any other node
Bias --> F
bias --> der

# To change the activation function of a node, specify the activation
# function followed by a list of nodes with that activation function.
# Any activation function in the pyneat.ActivationFunction enum can be
# used.
relu: der, F
"""

def parse_net(saved_string):
    template = net.parseString(saved_string, parseAll=True)[0]
    return template.make_genome()

if __name__=='__main__':
    parsed = parse_net(example)
    import IPython; IPython.embed()
