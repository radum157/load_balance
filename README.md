# README

## View as webpage
```
sudo pip3 install grip
grip  README.md
# open http://localhost:6419/
```

## Solution description

An array of *servers* and a hashtable for each server were used to implement the solution. Each server is functionally the same as a *hashtable* in which both the key and the value are `strings`. Additional functions were created to transfer data from one server to another.

After initialising, the load balancer can add or remove servers, as well as store and retrieve objects from the servers. *Note* that the balancer must be freed after usage.

The requested **server-wise functions** represent simple hashtable operations. The default number of `buckets` inside the hastables was set at `500`.

Further explanations can be found inside the header and source files (through comments, variable names etc). Each function is explicit in its behaviour, however brief descriptions were also provided.

**NOTE** that the hash ring was not created as an integer array containing hashes. The server array is considered to be the illustrated circle. See the implementation to clarify.
