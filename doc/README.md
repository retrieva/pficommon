pficommon documents
===

Build a Docker image.

```console
$ docker build --no-cache -t pficommon_doc .
```

Run `make html` to build HTML of the documents.

```console
$ docker run --rm -v ${PWD}:${PWD} -w ${PWD} pficommon_doc /bin/bash -c 'make html'
```

Serve built HTML at http://localhost:8080 .

```console
$ docker run --rm -dit -v ${PWD}/build/html:/usr/local/apache2/htdocs -p 8080:80 pficommon_doc
```
