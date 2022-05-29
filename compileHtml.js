const fs = require("fs");

(async () => {
    let data = await fs.promises.readFile("./index.html", "utf-8");
    // while (data.includes("\n"))
    //     data.replace("\n", " ");
    data = data.replaceAll("\r\n", " ")
    data = data.replaceAll("\"", "\\\"");
    await fs.promises.writeFile("./main/index.h", `String indexHtml = "${data}";`);
    await fs.promises.writeFile("./indexCompiled.html", `${data.replaceAll("\\\"", "\"")}`);
})()