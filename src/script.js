fetch("http://localhost:8080/calculate", {
    method: "POST",
    body: new URLSearchParams({expr: "2 + 3"})
}).then(r => r.json)
  .then(data => console.log(data.result))
  