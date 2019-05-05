[@bs.val] external require: string => string = "";
[@bs.val] [@bs.scope "performance"] external now: unit => float = "";

require("../../../src/App.scss");

type touches = {
  first: option((float, float)),
  last: option((float, float)),
};

type state = {
  isOpen: bool,
  touches,
  width: ref(float),
};

type action =
  | ToggleMenu(bool)
  | TouchStart(float)
  | TouchMove(float)
  | TouchEnd;

let component = ReasonReact.reducerComponent("App");

let make = _children => {
  ...component,
  initialState: () => {
    isOpen: false,
    touches: {
      first: None,
      last: None,
    },
    width: ref(0.0),
  },
  reducer: (action, state) =>
    switch (action) {
    | ToggleMenu(isOpen) => ReasonReact.Update({...state, isOpen})
    | TouchStart(clientX) =>
      Js.log2("Start", clientX);
      if (state.isOpen) {
        ReasonReact.Update({
          ...state,
          touches: {
            first: Some((clientX, now())),
            last: None,
          },
        });
      } else {
        ReasonReact.NoUpdate;
      };
    | TouchMove(clientX) =>
      if (state.isOpen) {
        ReasonReact.Update({
          ...state,
          touches: {
            ...state.touches,
            last: Some((clientX, now())),
          },
        });
      } else {
        ReasonReact.NoUpdate;
      }
    | TouchEnd =>
      if (state.isOpen) {
        let (x, t) =
          Belt.Option.getWithDefault(state.touches.first, (0.0, 0.0));
        let (x', t') =
          Belt.Option.getWithDefault(state.touches.last, (0.0, 0.0));
        let velocity = (x' -. x) /. (t' -. t);
        let state = {
          ...state,
          touches: {
            first: None,
            last: None,
          },
        };
        if (velocity < (-0.3) || x' < state.width^ /. 2.0) {
          ReasonReact.UpdateWithSideEffects(
            state,
            self => self.send(ToggleMenu(false)),
          );
        } else {
          ReasonReact.Update(state);
        };
      } else {
        ReasonReact.NoUpdate;
      }
    },
  render: self =>
    <div
      className={"App" ++ (self.state.isOpen ? " overlay" : "")}
      onClick={_event =>
        if (self.state.isOpen) {
          self.send(ToggleMenu(false));
        }
      }
      onTouchStart={event =>
        self.send(
          TouchStart(
            ReactEvent.Touch.changedTouches(event)##item(0)##clientX,
          ),
        )
      }
      onTouchMove={event =>
        self.send(
          TouchMove(
            ReactEvent.Touch.changedTouches(event)##item(0)##clientX,
          ),
        )
      }
      onTouchEnd={_event => self.send(TouchEnd)}>
      <header>
        <a
          onClick={event => {
            ReactEvent.Mouse.stopPropagation(event);
            self.send(ToggleMenu(true));
          }}>
          <img src={require("../../../src/img/icon/hamburger.svg")} />
        </a>
        <h1> {ReasonReact.string("Home")} </h1>
      </header>
      <nav
        className={self.state.isOpen ? "active" : ""}
        onClick={event => ReactEvent.Mouse.stopPropagation(event)}
        style={
          switch (self.state.touches) {
          | {first: Some((x, _)), last: Some((x', _))} =>
            ReactDOMRe.Style.make(
              ~transform=
                "translateX("
                ++ string_of_float(x' -. x > 0.0 ? 0.0 : x' -. x)
                ++ "0px)",
              ~transition="none",
              (),
            )
          | _ => ReactDOMRe.Style.make()
          }
        }>
        <header>
          <a onClick={_event => self.send(ToggleMenu(false))}>
            <img src={require("../../../src/img/icon/arrow.svg")} />
            {ReasonReact.string("Home")}
          </a>
        </header>
        <label> {ReasonReact.string("home")} </label>
        <ul> <li> <a> {ReasonReact.string("Home")} </a> </li> </ul>
        <label> {ReasonReact.string("pages")} </label>
        <ul>
          <li> <a> {ReasonReact.string("Page1")} </a> </li>
          <li> <a> {ReasonReact.string("Page2")} </a> </li>
          <li> <a> {ReasonReact.string("Page3")} </a> </li>
        </ul>
      </nav>
      <main> ReasonReact.null </main>
    </div>,
};