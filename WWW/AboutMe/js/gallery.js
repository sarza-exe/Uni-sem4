/*jslint browser:true */
/*global Image, console */

var count;

/*––– Data –––*/
var images = [
    {
        alt: "Blender render of a basement",
        src: "images/basement.png"
    },
    {
        alt: "Simple render of a ship on the sea",
        src: "images/Ship.png"
    },
    {
        alt: "Fairy-tale Inn in Blender",
        src: "images/Fairy inn.png"
    },
    {
        alt: "One of my first projects in Blender",
        src: "images/Tunnel.png"
    },
    {
        alt: "Cool mountain",
        src: "images/kpn.jpg"
    }
];

/*––– DOM refs –––*/
const gallery = document.getElementById("gallery");
const loader = document.getElementById("loader");
const modal = document.getElementById("imgModal");
const modalImg = document.getElementById("modalImg");
const caption = document.getElementById("caption");
const closeBtn = modal.querySelector(".close");

/*––– Image loader –––*/
function loadImage(params) {
    return new Promise(function (resolve, reject) {
        var img = new Image();
        img.alt = params.alt;
        img.src = params.src;
        img.onload = function () {
            resolve(img);
        };
        img.onerror = function () {
            reject(new Error("Cannot load " + params.src));
        };
    });
}

// kick off the load
const promises = images.map(loadImage);

/*––– Show pictures and hide loader when done –––*/
const total = promises.length;
count = 0;

promises.forEach(function (p) {
    p.then(function (img) {
        gallery.appendChild(img);
        count += 1;
        if (count === total) {
            loader.style.display = "none";
        }
    }).catch(function (err) {
        console.warn(err);
    });
});

/*––– Modal behavior –––*/
// open on click
gallery.addEventListener("click", function (e) {
    if (e.target.tagName !== "IMG") {
        return;
    }
    modal.style.display = "block";
    modalImg.src = e.target.src;
    caption.textContent = e.target.alt || "";
    e.stopPropagation();
});

// close on ×
closeBtn.addEventListener("click", function () {
    modal.style.display = "none";
});

// close on overlay click
modal.addEventListener("click", function (e) {
    if (e.target === modal) {
        modal.style.display = "none";
    }
});
