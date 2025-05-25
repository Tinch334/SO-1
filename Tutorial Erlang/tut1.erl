-module(tut1).

-export([start/0, say_something/2]).

say_something(_, 0) ->
    done;
say_something(Text, N) ->
    io:format("~p~n", [Text]),
    say_something(Text, N - 1).

start() ->
    spawn(tut1, say_something, [hello, 4]),
    spawn(tut1, say_something, [bye, 4]).