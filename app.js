const $ = (id) => document.getElementById(id);
const positions = [
  [90,190],[260,100],[260,285],[430,80],[430,280],[570,175],[665,85],[665,300]
];
let lastPath = [];

async function init() {
  const response = await fetch('/api/locations');
  const data = await response.json();
  data.locations.forEach((name, i) => {
    $('from').add(new Option(name, name));
    $('to').add(new Option(name, name));
  });
  $('from').value = data.locations[0];
  $('to').value = data.locations[1];
  drawGraph();
}
function drawGraph() {
  const edges = [[0,1,8],[0,2,5],[0,3,11],[1,2,4],[1,4,6],[1,5,9],[2,3,3],[2,4,7],[3,5,6],[4,5,4],[4,6,12],[5,6,10],[6,7,7],[5,7,14]];
  const edgeLayer = $('edges'), labelLayer = $('edgeLabels'), nodeLayer = $('nodes');
  edgeLayer.innerHTML = ''; labelLayer.innerHTML = ''; nodeLayer.innerHTML = '';
  edges.forEach(([a,b,w]) => {
    const [x1,y1] = positions[a], [x2,y2] = positions[b];
    edgeLayer.insertAdjacentHTML('beforeend', `<line id="edge-${a}-${b}" x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" class="edge"/>`);
    labelLayer.insertAdjacentHTML('beforeend', `<text x="${(x1+x2)/2}" y="${(y1+y2)/2-8}" class="weight">${w} km</text>`);
  });
  const names = [...$('from').options].map(o=>o.value);
  names.forEach((name,i)=>{
    const [x,y] = positions[i];
    nodeLayer.insertAdjacentHTML('beforeend', `<g class="node" id="node-${i}"><circle cx="${x}" cy="${y}" r="22"/><text x="${x}" y="${y+4}">${String.fromCharCode(65+i)}</text><text x="${x}" y="${y+39}" class="node-name">${name.replace('CGC University','CGC Univ.').replace('Mohali ','')}</text></g>`);
  });
}
function clearRoute() {
  document.querySelectorAll('.edge').forEach(e=>e.classList.remove('active'));
  document.querySelectorAll('.node circle').forEach(e=>e.classList.remove('active'));
}
function highlight(path) {
  clearRoute();
  for(let i=1;i<path.length;i++){
    const a=[...$('from').options].findIndex(o=>o.value===path[i-1]);
    const b=[...$('from').options].findIndex(o=>o.value===path[i]);
    const edge=document.getElementById(`edge-${Math.min(a,b)}-${Math.max(a,b)}`);
    if(edge) edge.classList.add('active');
  }
  path.forEach(name=>{
    const i=[...$('from').options].findIndex(o=>o.value===name);
    const node=document.querySelector(`#node-${i} circle`);
    if(node) node.classList.add('active');
  });
}
async function findRoute() {
  $('error').hidden=true;
  $('empty').hidden=true;
  $('routeResult').hidden=true;
  const from=$('from').value, to=$('to').value, algorithm=$('algorithm').value;
  if(from===to){$('error').textContent='Source and destination must be different.';$('error').hidden=false;return;}
  $('find').disabled=true; $('find').textContent='Calculating route…';
  try {
    const url=`/api/route?from=${encodeURIComponent(from)}&to=${encodeURIComponent(to)}&algorithm=${algorithm}`;
    const response=await fetch(url);
    const result=await response.json();
    if(!response.ok) throw new Error(result.error||'Could not calculate route.');
    if(!result.found) throw new Error('No route exists between these locations.');
    lastPath=result.path; highlight(result.path);
    $('distance').textContent=`${result.distanceKm} km`;
    $('time').textContent=`${result.estimatedMinutes} min`;
    $('resultSubtitle').textContent=`${algorithm.toUpperCase()} result`;
    $('steps').innerHTML=result.path.map((name,i)=>`<li>${name}</li>`).join('');
    const explanations={
      dijkstra:"Dijkstra uses a min-priority queue and repeatedly relaxes the nearest unvisited node. This result minimizes total road distance.",
      bfs:"BFS explores the graph level by level. It minimizes the number of road segments, not necessarily total kilometres.",
      dfs:"DFS explores one branch deeply before backtracking. It returns a valid traversal path, but does not guarantee minimum distance."
    };
    $('algorithmNote').textContent=explanations[algorithm];
    $('routeResult').hidden=false;
  } catch(err) {
    $('error').textContent=err.message; $('error').hidden=false; $('empty').hidden=true;
  } finally {
    $('find').disabled=false; $('find').innerHTML='Find best route <span>→</span>';
  }
}
$('find').addEventListener('click',findRoute);
$('swap').addEventListener('click',()=>{const a=$('from').value;$('from').value=$('to').value;$('to').value=a;});
$('algorithm').addEventListener('change',()=>document.querySelectorAll('.algo-card').forEach((c,i)=>c.classList.toggle('selected',['dijkstra','bfs','dfs'][i]===$('algorithm').value)));
init().catch(()=>{$('error').textContent='Cannot connect to C++ server. Please start server.exe first.';$('error').hidden=false;});
