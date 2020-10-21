const url = "https://static-links-page.signalnerve.workers.dev"

const links = [
    {"name":"Linkedin",
     "url":"https://www.linkedin.com/in/zisheng-jason/"},
    {"name":"My latest github project",
     "url":"https://github.com/jzisheng/News-Summarizer"},
    {"name":"Youtube",
     "url":"https://www.youtube.com/channel/UCMNBbsdZfwfRBWoDmjyvtCw"}
]

addEventListener('fetch', event => {
    event.respondWith(handleRequest(event.request))
})


/**
 * Respond with hello worker text
 * @param {Request} request
 */
async function handleRequest(request) {
    const routes = request.url.split('/')    
    if (routes.length == 4 && routes[3] == "links"){
	const json = JSON.stringify(links)
	return new Response(json, {
            headers: { 'content-type': 'application/json' }
	})
    }    
    else{
	return getHtml(request,url)
    }
}


/**
 * Function for getting rewriter 
 * transformed text/HTML for url page
 **/
async function getHtml(request,url){
    // scraping raw text/HTML from page
    const response = await fetch(url).then(function (response) {
	return response.text();
    }).catch(function (err) {
	console.warn('Something went wrong.', err);
    });

    // use html rewriter to transform html
    var res = await fetch(url);
    res = rewriter.transform(res)
    res = profileWriter.transform(res)
    res = imgTransformer.transform(res)
    res = nameTransformer.transform(res)
    
    res = socialWriter.transform(res)
    res = socialSvgWriter.transform(res)    
    res = headWriter.transform(res)
    res = bgTransformer.transform(res)
    res.headers.set('content-type','text/html')
    return res
}


/**
 * Method for transforming links array
 *  array to HTML 'a' links
 **/
function makeLinksHtml(links){
    var result = `<div id="links">\n`
    for(var i = 0; i < links.length; i++){
	var name = links[i]["name"]
	var url = links[i]["url"]	
	result += `  <a href="${url}">${name}</a>\n`
    }
    result += `</div>`
    return result
}

/**
 * HTML Rewriter class for links, profile, image, and name
 **/

// my selfie (:
const imgUrl = "https://datascience.duke.edu/sites/datascience.duke.edu/files/styles/people200x200/public/DSC_0344.jpg"

class AppendTextTransformer {
    constructor(text) {
	this.text = text
    }
    element(element) {
	element.append(this.text,{html:true})
    }
}


class LinksTransformer {
    constructor(links) {
	this.links = links
    }
    element(element) {
	const text = makeLinksHtml(links)	
	element.append(text,{html:true})
    }
}




class ProfileTransformer{
    constructor(){
    }
    element(element){
	element.removeAttribute("style")
    }
}

class AttributeTransformer{
    constructor(attrib,val){
	this.attrib = attrib
	this.val = val
    }
    element(element){
	element.setAttribute(this.attrib,this.val)
    }
}
class NameTransformer{
    constructor(name){
	this.name = name
    }
    element(element){
	element.setInnerContent(this.name)
    }
}

class SocialTransformer{
    constructor(){
    }
    element(element){
	const text = `<div id="links">
	  <a href="https://github.com/jzisheng">
	    <svg width="10%" height="10%" ><img src="https://simpleicons.org/icons/github.svg"></svg>
	  </a>
	</div>`	
	element.append(text,{html:true})
    }
}


// writer to add links
const rewriter = new HTMLRewriter()
      .on('div#links', new LinksTransformer(links))
// writer to show profile
const profileWriter = new HTMLRewriter()
      .on('div#profile', new ProfileTransformer())
// writer to add image
const imgTransformer = new HTMLRewriter()
      .on('img#avatar', new AttributeTransformer("src",imgUrl))
// writer to add profile username
const nameTransformer = new HTMLRewriter()
      .on('h1#name', new NameTransformer("jzisheng"))

// writer to show social links
const socialWriter = new HTMLRewriter()
      .on('div#social', new ProfileTransformer())
// writer to add SVGs to social links
const socialSvgWriter = new HTMLRewriter()
      .on('div#social', new SocialTransformer())
// writer to update HTML head
const headWriter = new HTMLRewriter()
      .on('title', new NameTransformer("Jason Chang"))

const bgTransformer = new HTMLRewriter()
      .on('body', new AttributeTransformer("class","bg-pink-300"))
