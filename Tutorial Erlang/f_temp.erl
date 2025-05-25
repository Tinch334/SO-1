-module(f_temp).

-export([format_temps/1]).

format_temps([]) ->
    ok;
format_temps([City | Rest]) ->
    print_temp(convert_c(City)),
    format_temps(Rest).

convert_c ({Name, {f, Temp}}) ->
    {Name, {c, (Temp - 32) * 5 / 9}};
convert_c (V) ->
    V.

print_temp({Name, {c, Temp}}) ->
    io:format("~-15w ~w c~n", [Name, Temp]).