/*
 * data.h
 *
 *  Description: test data
 */

#ifndef DATA_H_
#define DATA_H_

// #pragma NOINIT(squirtle64)
// const char squirtle64[] = "iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAACOHpUWHRSYXcgcHJvZmlsZSB0eXBlIGV4aWYAAHjanVRrkhwhCP7vKXIEeYh6HG21KjfI8fP56OxOZ3YrFRyVRgQ+wHH918/hfkyKYk5DTJbNPEizZi5gkt80+YrJmG2LkvmbyOe1LnJLeYtfDm458UN+dioPQ3q8U309uOWcHoZ4bzIdgCc9hvJhSCB+49lyil4/vstxMA5Ei0EtxeCsGbMX4S6GSdub7FmE1soSoUMSwXsJWz6tzAXe48zRiWhdZRiiTPPgli8AcpL2SS5+OVnJmrbcE4rnffBMOgJ7L/d31WyZp+X1VrL3lfmuMO67yixYnxKtejh+lWfz5D6HN0ZLY/SloUUNDWqnw27wdPTQpjoxBCDRORyYsFnscxhSeqG34xkDvV0R70XkBzHG3CcJGV0UsSsVR2EJE84TFWoYYw2hyMzKTJ0VFzpk/U8sdNzG5bj75OC7Q0lhTKn//3D/qjjGNVNE60GXNREXz7JMsEi2zA1qqALF02vkvyZdgNK+wfspoHncyvrp3QftnsZFwJ7PUxiJWzlJpOF2HlZVX8v/F8HQUU9EJ9YvonUfvgU+2kR/HXAT+wQOa2i6EVMdBQotynzgpo07X5xjiNmqi6xiPaeaZNSUQwm4n0tqY9Yyew0ZT7/U0XuVdvUU2EhbnIdWrY3UO+cSXUCGIwXWlmCsS6XC5kOwEmEvZqJeEGzW0WIrVWyUjH+Y6+JK3Y8Qu7UWhrp6Zdb9G1iblM4suVgpqEzL7jdrukqUey5daAAAAARzQklUCAgICHwIZIgAAAFZSURBVBiVbdC9SwJxHAbw7+9+6mkeXepQohhR0eTUG9QU/QfBOQdt0dLLH2BzS0tE/0GcbdIkxGWpi5qZXlKUZhHmaZkevqTet0EKB5/x4QMPPAB9eZNl2y3iDAwIg4gMAIB4HnCHqCUSjKTjh9LNASIyiEj+oVeSGEEUaVFjNyuUnc5dSEOPha+d00zGRQjBP8zIioI+j6fbMujyiaSMxSk3Gn/q34vvTwg91IOiIGhHV3d7DufYhmD5gHlO1XibdSSajG8BIUhIb50Bn1ev1Br24WaBzrk4pDqqmaBNnDy/jIhGRAQAAB2koePdX9hNxALWhobrbmMFXj6rmG9RTgrdhxtq7VVMpQQCABCLpceDHTZnL8dVjpLuw+gSn80+Q9tsAUOtLG9btVkdAICilrRqnT3OlOwna5Nst6oULs0EzqhaDLtMeD2xstoc9C2Ifr8Do1F9f/cLcA6jov2Ac18AAAAASUVORK5CYII=";
const char squirtle64[] = "iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAACOHpUWHRSYXcgcHJvZmlsZSB0eXBlIGV4aWYAAHjanVRrkhwhCP7vKXIEeYh6HG21KjfI8fP56OxOZ3YrFRyVRgQ+wHH918fasefsebasrbsdhdftjdfyjfgmghj,fy,udtymksrtyjntgnjdfmkdtgm,dg,mdfmdrtjnsebhsetned/h"; 

// #pragma NOINIT(charmander64)
// const char charmander64[] = "iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAACMnpUWHRSYXcgcHJvZmlsZSB0eXBlIGV4aWYAAHjanVTZkRwhDP0nCoeADgSEAzRUOQOH7yeatnfH6ymXpQE0Quihgw7zx/cVvjlliUFTLlbNIkirVm4QSrzJ5Y7BGNetKhYfolj3vCls41v9aePRE7/oz0rtxZEedOqfNx49lxdHfC/iAJBJj6N6BBKov0C2WnLU3//bAVgnRMtJreQU7DLmKMJTDINuNLlHE9ozS4YNSYYcJWFOsr34BPTsOTo32kcZjqiSbzz6HYCcpH3QS9wgO1nuK7yGEvneeE06Lva1Pj5Vs+2eNupjZF9X5l1hwrvK7LA+JFr1SPxZj6MUPl5vrausNbeFNjU0qJ0Oe4KnY4c2VY8hIRJ1DhDSLWJ1NqR0oLfz4YXe7gAdRHERg311EjIalLEqtUBpKwv2CzW6wGuzUGZmZabJigMTuvnrLnRg8waesQRgTxgpnCnN/+fwr4ZrDU8R7Qfd9sC92MviwSLZ4gvMUAV6Ukvx7+QPJt32fD8Ebx0km3e8f9Ld0V5kZJH9JzsfxXNrD7Q+sOENthzjQrexvLnp7iMgMxAuj3wcc4/bg4Y3jNQ5N1t2LcaM5x37kHgNKWVp7p2rBDET3F1b9fffspQ26hU5IauKU5VGXPOKdA1qRrUuplbayrlHfD20Wr2GSQ1N8Q3kTitV6q3hA3HxtGIpoaMywc5ULXW5jCZad6XWLVEZU0qVzjK5Sys9+LclzYHjg2nUhRBr+Amjo0ab621vGwAAAARzQklUCAgICHwIZIgAAAFlSURBVBiVbZA9L0MBGIXPe92rX1LSqo+SILE0RAWD2MQgYhMWMYihP8AfuG4iJDpYbBISo0UwMJBrkCZU02p0oESUNqRF3VIt7X0NRQye4Yzn5DzAH2IBtZWZBfyDwMwEAFpgfaG+GA1dbS16AYCZiWVZYAYBKMXY8q5NtgVjbQ7dsuTTXts72l0HR5cJRVH030YAmGgRpbdMPnUTJ242S7lkjSujKIr+uLMy+LA27y5NA9Rtlcrt1fbDeKGCWhwGGs74pk/VzV5LITRrrkqPA4BAAOeysR5HtanRPTTw2eR22180zUNFqhObTemiWRIAQGRmgYg24rEoO6sq+/KFLMpM14lM7iN4HGk4N5cbl0uNRPoJs6SHt72J4z3pLp5+M3xoXdkb/8U7l6U6RzwXsiwLIgDQ9bPz9tMazYcjc4Weqf3kk39FrHXd949OzrAsC/TzXlVV8a/cVWaj7yxt4299APAFx+abpqao5nEAAAAASUVORK5CYII=";
const char charmander64[] = "iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAACMnpUWHRSYXcgcHJvZmlsZSB0eXBlIGV4aWYAAHjanVTZkRwhDP0nCoeADgSEAzRUOQOH7yeatnfH6ymXpQE0Quihgw7zx/c";

// #pragma NOINIT(bulbasaur64)
// const char bulbasaur64[] = "iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAACFXpUWHRSYXcgcHJvZmlsZSB0eXBlIGV4aWYAAHjanVRtriQhCPzvKfYIgoh6HNuPZG/wjv8Ktd/OTGYnm9XY0ogUFHS78fV7ul82khcnMWUtqh5DihSuELLfw+QLi7H6VmX19yBf1nMNt4y3+ung1hO/6M9O9cWRHHS6ng9uPecXR7y3YACQSY6jcgQKUL9B1pLBwJ/3egDmSVFTFM0pOu3K7EPgERSLNlrYqwZaTw4JNhQSZB/i0hjocgYhGUcnonWV4YgK2cGtXwmEQ9qDPvgFssgyX+41Fc/74JV0BPZe7++q6XJPC/U20veV+VQY96kyK60HokWOxM/6pJ7cY3hz9jznWBZSRdGgejrsTp6OHdpULIeITMSmgxC3iN2mgtKG3k5nTvT2hXgbkZ/EmLbbCKTUKGEXqo7iUmacZ6rUMeeagRIzCzMNFlwY0I2fWOjApgU8fHbAHjASOBMa/z/dvxrO2YwiWh90XQtxsZXFkgXZwTaYoQoUH6j922CftrXdX61jd4J1dnhjvvvZSgwOjQcKATwruP0BpUdQ9xYVCMc8L9jwMU6y4JyFiFpY1m1d2DFbwvCGr+Kamuelqft4XUl4kM5SrqG9z8QNItUWqxudS6MSWy3oxOEL/jMyVTKnmcBZ66WNWIri79mu1jm3mpskH7MBpdy9XDF6h28j4kLq2eO4j6wwAHIneM+FDRgRF/cNX7M5oyZg7p0AAAAEc0JJVAgICAh8CGSIAAABO0lEQVQYlY2QP0sCcRyHP7/fed4ZpRxWcA5KVkIRTuFQQ70DXVxaa2nrHfgGegnR4mZThFFT0CAoJEURdZCQyPmvMFPP8/59W3L3WZ9neoB/iIgREcsWsgJmhXLE8/m8enF/mSQiBgC+qaxq1SWVq+xavzo595/tT7gbrX/pLcbS2wDAAKBQLwXs3rjSbOkqhcUwXAsj8xPfzWF/U07sHGcOXzkALL+PsvZouNUWrfBaR3Bt7cd7fGpYtmCGdFs/AAAOAA2jt8rBEFtYnFDPYI7psmAgxt2B5HDPeZuGzEz4T+t6uzx86Ug3/S43IvNYiak+2afS7nq6DAB87y4nHG1kBrIk3U4ishYUxaLilxk3PG3OH/gdW10HABgRMQbg46EWjCsKEFdYsVJKRi3v2ZRCSiqVrM38FQD+AEQrhF5zghl5AAAAAElFTkSuQmCC";
const char bulbasaur64[] = "iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAACFXpUWHRSYXcgcHJvZmlsZSB0eXBlIGV4aWYAAHjanVRtriQhCPzvKfYIgoh6HNuPZG/wjv8Ktd/OTGYnm9XY0ogUFHS78";

const char *squirtle_ptr = squirtle64;
const char *charmander_ptr = charmander64;
const char *bulbasaur_ptr = bulbasaur64;

#endif /* DATA_H_ */