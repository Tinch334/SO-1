-module(tut2).
-export([start/0, ping/2, pong/0]).

ping(0, Pong_PID) ->
    Pong_PID ! done,
    io:format("Ping finished~n", []);

ping(N, Pong_PID) ->
    Pong_PID ! {ping, self()}, %The self() function returns the processes PID.

    receive
        pong ->
            io:format("Ping received pong~n", [])
    end,
    ping(N - 1, Pong_PID).


pong() ->
    receive
        done ->
            io:format("Pong received done~n", []);
        {ping, Ping_PID} ->
            io:format("Pong received ping~n", []),
            Ping_PID ! pong,
            pong()
    end.


start() ->
    Pong_PID = spawn(tut2, pong, []),
    spawn(tut2, ping, [5, Pong_PID]).